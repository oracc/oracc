(load-file "~/cdl-init.el")

(custom-set-variables
  ;; custom-set-variables was added by Custom.
  ;; If you edit it by hand, you could mess it up, so be careful.
  ;; Your init file should contain only one such instance.
  ;; If there is more than one, they won't work right.
 '(cua-mode t nil (cua-base))
 '(emacsw32-max-frames t)
 '(emacsw32-style-frame-title t)
 '(menuacc-mode t)
 '(nxhtml-load t)
 '(recentf-mode t)
 '(hfyview-quick-print-in-files-menu t)
 '(w32-meta-style (quote w32)))

;;; Type 'y' instead of 'yes'
(fset 'yes-or-no-p 'y-or-n-p)

;; Default font and ATF font are set to DejaVu Mono. The -17- stands for the font size; if you like a bigger or smaller font, simply increase or decrease this number.
(set-default-font       "-outline-Dejavu Sans Mono-normal-r-normal-normal-17-*-*-*-c-*-iso10646-1")
(reset-atf-default-font "-outline-Dejavu Sans Mono-normal-r-normal-normal-17-*-*-*-c-*-iso10646-1")
(global-visual-line-mode 1)

;; Displaying line and column info on the modeline: 
;; Show the line number in each modeline. 
(line-number-mode 1)
;; Show the column number in each modeline
(column-number-mode 1)

;; Select whole buffer with c-a
(global-set-key "\C-a" 'select-all)

(defun select-all ()
  (interactive)
  (set-mark (point-min))
  (goto-char (point-max)))

;; Alternatively use c-f to search
(global-set-key "\C-f" 'isearch-forward)

;; Alternatively use c-q to search and replace
;; unfortunately c-h seems to be reserved for some help-queries.
(global-set-key "\C-q" 'query-replace)

;; pc-bufsw.el allows c-tab and c-s-tab switching
;; Author: Igor Boukanov <boukanov@fi.uib.no> 
;; http://ftp.sunet.se/pub/gnu/emacs-lisp/incoming/pc-bufsw.el
(require 'pc-bufsw)
(pc-bufsw::bind-keys [C-tab] [C-S-tab])

;; w32-winprint.el allows you to print the buffer with Notepad
;; Author: Lennart Borgman
(require 'w32-winprint)

