(defun oracc-project-action (act)
  (interactive)
  (let
      ((proj (cbd-project-from-buffer)))
    (if proj
	(cbd-shell-command (concat "oracc " act))
      (message "buffer file is not part of a project"))))

(defun oracc-project-check ()
  (interactive)
  (oracc-project-action "check"))

(defun oracc-project-harvest ()
  (interactive)
  (oracc-project-action "harvest"))

(defun oracc-project-merge ()
  (interactive)
  (oracc-project-action "merge"))

(defun oracc-project-rebuild ()
  (interactive)
  (oracc-project-action "rebuild"))

(defvar oracc-project-mode-map
  (let ((oracc-project-mode-map (make-sparse-keymap)))
    oracc-project-mode-map)
  "Keymap for ORACC Project minor mode")

(define-minor-mode oracc-project-mode
  "Minor mode for accessing ORACC project management tasks.
  With no argument, this command toggles the mode.
  Null prefix argument turns off the mode.

  When ORACC Project mode is enabled, a menu is provided which
  facilitates access to project-management tasks.

  Nothing is added to the mode-line by this mode."
  :init-value nil
  :lighter nil
  :global t
  nil
  (when oracc-project-mode
    (easy-menu-define oracc-project-mode-menu oracc-project-mode-map "" 
      (list "ORACC" 
	    ["Check Project" oracc-project-check t]
	    ["Harvest Glossary" oracc-project-harvest t]
	    ["Merge Glossaries" oracc-project-merge t]
	    ["Rebuild Project" oracc-project-rebuild t]
	    ["ORACC Project help" describe-mode t]
	    ))))

