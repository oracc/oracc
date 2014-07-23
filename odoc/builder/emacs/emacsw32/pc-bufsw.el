;;; pc-bufsw.el -- Support for a quick switch between Emacs buffers.

;; Author: Igor Boukanov <boukanov@fi.uib.no>
;; Maintainer: Igor Boukanov
;; Version 0.9
;; Keywords: buffer

;; Copyright (C) 1997, 1998 Igor Boukanov.

;; No guarantees or warantees or anything are provided or implied in any 
;; way whatsoever. Use this code at your own risk. Permission to use this 
;; code for any purpose is given, as long as the copyright is kept intact. 

;;; Commentary:
;; This is an implementation of quick switch for Emacs buffer that is similar 
;; to one that is widely used on PC. The main idea here is that a user chooses
;; two keys (most often C-tab and S-C-tab) that switch between buffers 
;; according to most or least recently order. After the final choice is made 
;; the last selected buffer becomes most recently used one. 

(provide 'pc-bufsw)

(defconst pc-bufsw::quite-time 3
  "Quite time to automaticaly terminate buffer switch mode. 
If there is no input during quite-time seconds makes the last choosen 
buffer current." ) 

; Variable to store data vector during buffers change.
; It has the following structure:
;   element at index 2*i - buffer to select  after i'th switch
;   element at index 2*i+1 - window that initially shows buffer 2*i or nil
; The vector may contain any buffer refference several times if that was shown
; initially in several windows. It is supposed that buffers in the vector are 
; odered according to most recently used oder.
(defvar pc-bufsw::walk-vector nil)

; Index of currently selected buffer in pc-bufsw::walk-vector. Always even.
(defvar pc-bufsw::cur-index 0)

; The initial buffer list. It is used to construct 
; pc-bufsw::walk-vector. The buffer oder after an user stops 
; selection much the pc-bufsw::start-buf-list except that the selected buffer 
; is moved on the top.
(defvar pc-bufsw::start-buf-list 0)

(defun pc-bufsw::previous () 
  (interactive) 
  (pc-bufsw::walk 1))

(defun pc-bufsw::lru () 
  (interactive) 
  (pc-bufsw::walk -1))

;;;###autoload
(defun pc-bufsw::bind-keys (key1 key2)
  "Bind key1 and key2 to switch buffers in most or least recently used oder.
Pressing key1 or key2 would switch to most or least recently used buffer and
enter switch mode. In this mode subsequent pressing of key1 or key2 would go 
father in buffer list shown in echo area. 
Pressing any other key or no input during the period indicated 
by 'pc-bufsw::quite-time' variable closes the mode and makes the last selected buffer current. 
If newly selected buffer is shown in some window that would be used to show
the buffer. Otherwise it will be displayed in the initial window."
  (global-set-key key1 'pc-bufsw::previous) 
  (global-set-key key2 'pc-bufsw::lru)) 

(defun pc-bufsw::walk (direction) 
  (catch 'cancel
    (when (null pc-bufsw::walk-vector) (pc-bufsw::start))
    (pc-bufsw::choose-next-index direction)
    (pc-bufsw::choose-buf-and-wnd)
    (pc-bufsw::show-buffers-names)
    (if (sit-for pc-bufsw::quite-time) (pc-bufsw::finish))))

(defun pc-bufsw::start () 
  (setq pc-bufsw::start-buf-list (buffer-list))
  (setq pc-bufsw::cur-index 0)
  (setq pc-bufsw::walk-vector (pc-bufsw::get-walk-vector))
  (add-hook 'pre-command-hook 'pc-bufsw::switch-hook))

;; Hook to access next input from user.
(defun pc-bufsw::switch-hook () 
  (if (not (or (eq 'pc-bufsw::lru this-command)
	       (eq 'pc-bufsw::previous this-command)))
      (pc-bufsw::finish))) 

;; Construct main buffer/window vector. Throw 'cancel if the current buffer 
;  is minibuffer. 
(defun pc-bufsw::get-walk-vector()
  (let ((wnd (selected-window)))
    (when (window-minibuffer-p wnd) (throw 'cancel nil))
    (let ((v (pc-bufsw::get-walk-vector-impl pc-bufsw::start-buf-list 2 wnd)))
      (aset v 0 (window-buffer wnd))
      (aset v 1 wnd)
      v)))

(defun pc-bufsw::get-walk-vector-impl(tail count start-wnd)
  (if (null tail) (make-vector count nil)
    (let ((buffer (car tail))
	  (tail (cdr tail)))
      (if (char-equal ?\  (aref (buffer-name buffer) 0))
	  (pc-bufsw::get-walk-vector-impl tail count start-wnd)
	(let ((wnd-list (get-buffer-window-list buffer 0 t)))
	  (if (null wnd-list) (let ((v (pc-bufsw::get-walk-vector-impl 
					tail (+ count 2) start-wnd)))
				(aset v count buffer)
				(aset v (1+ count) nil)
				v)
	    (let ((len (length wnd-list)))
	      (when (memq start-wnd wnd-list) (setq len (1- len)))
	      (let ((v (pc-bufsw::get-walk-vector-impl 
			tail (+ count (* 2 len)) start-wnd)))
		(while wnd-list
		  (let ((wnd (car wnd-list)))
		    (when (not (eq wnd start-wnd))
		      (aset v count buffer)
		      (setq count (1+ count))
		      (aset v count wnd)
		      (setq count (1+ count))))
		  (setq wnd-list (cdr wnd-list)))
		v))))))))


;; Echo buffer list. Current buffer marked by <>.
(defun pc-bufsw::show-buffers-names()
  (let* ((width (frame-width))
	 (n (pc-bufsw::find-first-visible width))
	 (str (pc-bufsw::make-show-str n width)))
    (message "%s" str)))

(defun pc-bufsw::find-first-visible(width)
  (let ((first-visible 0)
	(i 2)
	(visible-length (pc-bufsw::show-name-len 0 t)))
    (while (<= i pc-bufsw::cur-index)
      (let ((cur-length (pc-bufsw::show-name-len i (= first-visible i))))
	(setq visible-length (+ visible-length cur-length))
	(if (> visible-length width)
	    (progn
	      (setq first-visible i)
	      (setq visible-length cur-length))))
      (setq i (+ 2 i)))
    first-visible))

(defun pc-bufsw::show-name-len(i at-left-edge)
  (+ (if at-left-edge 2 3) 
     (length (buffer-name (aref pc-bufsw::walk-vector i)))))

(defun pc-bufsw::make-show-str (first-visible width)
  (let* ((i (+ 2 first-visible))
	 (count (length pc-bufsw::walk-vector))
	 (str (pc-bufsw::show-name first-visible t))
	 (visible-length (length str))
	 (continue-loop (not (= i count))))
    (while continue-loop
      (let* ((name (pc-bufsw::show-name i nil))
	     (name-len (length name)))
	(setq visible-length (+ visible-length name-len))
	(if (> visible-length width) (setq continue-loop nil)
	  (setq str (concat str name))
	  (setq i (+ 2 i))
	  (when (= i count) (setq continue-loop nil)))))
    str))

(defun pc-bufsw::show-name(i at-left-edge)
  (let ((name (buffer-name (aref pc-bufsw::walk-vector i))))
    (cond ((= i pc-bufsw::cur-index) (concat (if at-left-edge "<" " <") 
						 name ">"))
	  (at-left-edge (concat " " name " "))
	  (t (concat "  " name " ")))))

(defun pc-bufsw::choose-next-index (direction)
  (setq pc-bufsw::cur-index
	(mod (+ pc-bufsw::cur-index (* 2 direction)) 
	     (length pc-bufsw::walk-vector))))

(defun pc-bufsw::choose-buf-and-wnd () 
  (let ((buf (aref pc-bufsw::walk-vector pc-bufsw::cur-index))
	(wnd (aref pc-bufsw::walk-vector (1+ pc-bufsw::cur-index)))
	(cur-wnd (selected-window)))
    (when (null wnd) (setq wnd (aref pc-bufsw::walk-vector 1)))
    (when (not (eq cur-wnd wnd))
      (when (eq cur-wnd (aref pc-bufsw::walk-vector 1))
	(set-window-buffer cur-wnd (aref pc-bufsw::walk-vector 0)))
      (let ((wnd-frame (window-frame wnd)))
	(when (not (eq wnd-frame (selected-frame)))
	  (select-frame wnd-frame)
	  (raise-frame wnd-frame)))
      (select-window wnd))
    (switch-to-buffer buf t)))

;; Called on switch mode close
(defun pc-bufsw::finish()
  (pc-bufsw::restore-order (aref pc-bufsw::walk-vector 
				 pc-bufsw::cur-index) 
			   pc-bufsw::start-buf-list)
  (remove-hook 'pre-command-hook 'pc-bufsw::switch-hook)
  (setq pc-bufsw::walk-vector nil)
  (setq pc-bufsw::cur-index 0)
  (setq pc-bufsw::start-buf-list nil)
  (message nil))


;; Put buffers in Emacs buffer list according to oder indicated by list
;  except put chosen-buffer to the first place.
(defun pc-bufsw::restore-order(chosen-buffer list)
  (while list 
    (let ((buffer (car list)))
      (if (not (eq buffer chosen-buffer)) (bury-buffer buffer)))
    (setq list (cdr list))))


















