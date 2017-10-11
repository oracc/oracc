(defvar cbd-mode nil
  "Non-nil means that CBD mode is enabled.")

(make-variable-buffer-local 'cbd-mode)

(defvar cbd-mode-hook nil)

(defvar cbd-mode-map
  (let ((cbd-mode-map (make-sparse-keymap)))
    (define-key cbd-mode-map "\C-c\C-c" 'cbd-check)
    (define-key cbd-mode-map "\C-c\C-r" 'cbd-rebuild)
    (define-key cbd-mode-map "\C-c\C-x" 'cbd-xff)
    (define-key cbd-mode-map "\C-c\C-n" 'next-error)
    cbd-mode-map)
  "Keymap for CBD major mode")

(add-to-list 'auto-mode-alist '("\\.glo" . cbd-mode))

(defun cbd-shell-command (cmd)
  (interactive)
  (shell-command (concat "cd ~ ; PATH=$PATH:/usr/local/oracc/bin " cmd)))

(defun cbd-check ()
  (interactive)
  (let
      ((proj (cbd-project-from-buffer)))
    (if proj
	(cbd-shell-command "oracc check gloss")
      (message "buffer file is not part of a project"))))

(defun cbd-project-from-buffer ()
  (interactive)
  (if (string-match "/home/\\([a-z/]+\\)/00" (buffer-file-name))
      (match-string 1 (buffer-file-name))
    nil))

(defun cbd-lang-from-glo-buffer ()
  (interactive)
  (if (string-match "\\([a-z]+\\).glo$" (buffer-file-name))
      (match-string 1 (buffer-file-name))
    nil))

(defun cbd-xff ()
  (interactive)
  (let
      ((proj (cbd-project-from-buffer))
       (lang (cbd-lang-from-glo-buffer)))
    (if (and proj lang)
	(cbd-shell-command (concat "oracc -lang " lang " xff"))
      (if proj
	  (message "buffer file does not have .glo extension")
	(message "buffer file is not part of a project")))))

(defun cbd-rebuild ()
  (interactive)
;  (let
;      ((proj (cbd-project-from-buffer))
;       (lang (cbd-lang-from-glo-buffer)))
;    (if (and proj lang)
;	(ssh (concat proj "@oracc.museum.upenn.edu PATH=$PATH:/usr/local//bin oraccproject -lang " lang " rebuild gloss"))
;      (if proj
;	  (message "buffer file does not have .glo extension")
;	(message "buffer file is not part of a project")))))
  (message "sorry, rebuilding an individual glossary is not yet supported"))

(defface cbd-entry-face
  `((t (:foreground "darkslateblue")))
  "Face for @-tags in CBD files")

(defface cbd-form-face
  `((t (:foreground "red")))
  "Face for @form-lines in CBD files")

(defface cbd-comment-face
  `((t (:foreground "darkslateblue")))
  "Face for #-lines in CBD files")

(defvar cbd-entry-face 'cbd-entry-face)
(defvar cbd-form-face 'cbd-form-face)
(defvar cbd-comment-face 'cbd-comment-face)
(defvar cbd-bold-face 'cbd-comment-face)

(defconst cbd-font-lock-keywords-1
  (list
   '("^@[a-z]+" . cbd-entry-face)
   '(" .*\\[.*\\]" cbd-bold-face)
   ))
;   '("^@sense.*" . cbd-form-face)
;   '("^#.*" . cbd-comment-face)))

(defvar cbd-font-lock-keywords cbd-font-lock-keywords-1 
  "Patterns for CBD syntax colouring")

(defun cbd-mode (&optional arg)
  "Major mode for editing CBD glossaries.

CBD mode colourizes CBD files, enables some keyboard shortcuts, and
gives easy access to checking and rebuilding methods.

For the CBD documentation see URL `http://oracc.museum.upenn.edu/ns/cbd/1.0/'."
  (interactive "P")
  (let ((on-p cbd-mode))
    (kill-all-local-variables)
    (setq cbd-mode
	  (if (null arg)
	      (not on-p)
	    (> (prefix-numeric-value arg) 0)))
    (when cbd-mode
      (use-local-map cbd-mode-map)
      (easy-menu-define cbd-mode-menu cbd-mode-map "" 
      			(list "CBD" 
			      ["Check CBD" cbd-check t]
			      ["Lemmer update" cbd-xff t]
			      ["Project maintenance mode" oracc-project-mode t]
			      ["Rebuild" cbd-rebuild t]
			      ["CBD mode help" describe-mode t]
			      ["Browse CBD Docs" cbd-browse-doc t]
			      ))
      (set (make-local-variable 'font-lock-defaults) 
	   '(cbd-font-lock-keywords))
      (set (make-local-variable 'font-lock-string-face)
	   nil)
      (setq major-mode 'cbd-mode)
      (setq mode-name "CBD")
      (set-frame-font atf-default-font)
      (turn-on-cuneitex)
      (cbd-otl-mode)
      (run-hooks 'cbd-mode-hook))))

(defun cbd-otl-mode ()
  (add-hook 'outline-minor-mode-hook
	    '(lambda ()
	       (define-key outline-minor-mode-map [(shift tab)] 'outline-cycle)
	       (setq outline-regexp "\\(@let\\|@ent\\)")
	       (set-display-table-slot standard-display-table 
				       'selective-display 
				       (string-to-vector atf-otl-string))
	       (setq outline-level
		     '(lambda ()
			(cond ((looking-at "@l") 1)
			      ((looking-at "@e") 2)
			      (t 1000))))))

  (outline-minor-mode)
  )

(provide 'cbd-mode)
