;; ORACC version 1.1

(require 'compile)

(setq http-oracc-build (concat "http://" oracc-build "/"))

(defvar atf-mode nil
  "Non-nil means that ATF mode is enabled.")

(make-variable-buffer-local 'atf-mode)

(defvar atf-mode-hook nil)

(defvar atf-mode-map
  (let ((atf-mode-map (make-sparse-keymap)))
;    (define-key atf-mode-map "\C- " 'next-line)
    (define-key atf-mode-map "\M-." 'atf-brack-ellipsis)
;    (define-key atf-mode-map "c" 'atf-sz)
;    (define-key atf-mode-map "C" 'atf-SZ)
    (define-key atf-mode-map "[" 'atf-square)
    (define-key atf-mode-map "(" 'atf-round)
    (define-key atf-mode-map "{" 'atf-curly)
    (define-key atf-mode-map "<" 'atf-angle)
    (define-key atf-mode-map "|" 'atf-pipes)
    (define-key atf-mode-map "\C-c\C-c" 'atf-check)
    (define-key atf-mode-map "\C-c\C-d" 'atf-browse-doc)
    (define-key atf-mode-map "\C-c\C-l" 'atf-lemmatize)
    (define-key atf-mode-map "\C-c\C-n" 'next-error)
    atf-mode-map)
  "Keymap for ATF major mode")

(add-to-list 'auto-mode-alist '("\\.atf" . atf-mode))

(defface atf-andline-face
  `((t (:foreground "green4")))
  "Face for &-lines in ATF files")

(defface atf-block-face
  `((t (:foreground "red2")))
  "Face for @-lines in ATF files")

(defface atf-dollar-face
  `((t (:foreground "darkorchid")))
  "Face for $-lines in ATF files")

(defface atf-comment-face
  `((t (:foreground "darkslateblue")))
  "Face for #-lines in ATF files")

(defface atf-comment-bis-face
  `((t (:foreground "gray80")))
  "Face for ##-lines in ATF files")

(defface atf-comment-ls-face
  `((t (:foreground "MediumPurple1")))
  "Face for ###ls.-lines in ATF files")

(defface atf-linkline-face
  `((t (:foreground "royalblue")))
  "Face for ||, << and >> lines in ATF files")

(defvar atf-andline-face 'atf-andline-face)
(defvar atf-block-face 'atf-block-face)
(defvar atf-dollar-face 'atf-dollar-face)
(defvar atf-comment-face 'atf-comment-face)
(defvar atf-comment-bis-face 'atf-comment-bis-face)
(defvar atf-comment-ls-face 'atf-comment-ls-face)
(defvar atf-linkline-face 'atf-linkline-face)

(defconst atf-font-lock-keywords-1
  (list
   '("^.&[PQX].*" . atf-andline-face)
   '("^&.*" . atf-andline-face)
   '("^@[a-zA-Z0-9]+" . atf-block-face)
   '("^@([a-zA-Z0-9 ]+)" . atf-block-face)
   '("^$.*" . atf-dollar-face)
   '("^||.*" . atf-linkline-face)
   '("^<<.*" . atf-linkline-face)
   '("^>>.*" . atf-linkline-face)
   '("^###ls.*" . atf-comment-ls-face)
   '("^##.*" . atf-comment-bis-face)
   '("^#.*" . atf-comment-face)))

(defvar atf-font-lock-keywords atf-font-lock-keywords-1 
  "Patterns for ATF syntax colouring")

(defun atf-brack-ellipsis ()
  "Insert square brackets and ellipsis."
  (interactive)
  (insert "[...]"))

(defun atf-sz ()
  "Insert sz (when c is pressed)"
  (interactive)
  (atf-sz-SZ "c" "sz"))

(defun atf-SZ ()
  "Insert SZ (when C is pressed)"
  (interactive)
  (atf-sz-SZ "C" "SZ"))

(defun atf-sz-SZ (c sz)
  (let ((c-or-sz (save-excursion 
		   (beginning-of-line)
		   (if (looking-at "[\@\#\&\$]")
		       c
		     sz))))
    (insert c-or-sz)))

(defun atf-mode (&optional arg)
  "Major mode for editing ATF documents.

ATF mode colourizes ATF files, enables some keyboard shortcuts, and
gives easy access to the ATF template generator and checker.

KEYBOARD:
=========

In ATF mode the keyboard automatically inserts pairs of brackets
when any of [,(,<,{ are pressed and inserts a pair of vertical
bars when | is pressed; in all these cases the cursor is left
between the brackets or |.

ESC-. (escape-key period) inserts [...]

SELECTIVE DISPLAY:
==================

ATF mode automatically switches on outline-minor-mode, and sets
the outline headings so that &-lines are the top-level; @-lines
are the second level and text-lines are the third level.  The
keybinding shift-tab is defined to use Carsten Dominik's
outline-cycle function. By default, when text is hidden by
outline mode the string ` ' (space) is appended to the displayed
heading line--this means that there is no visual cue that the
text is hidden.  If you prefer to have such a cue, you can set
this in your .emacs file.  For example, to set the string to
`...', put the following line in your .emacs:

  (setq atf-otl-string \"...\")

Note that the behaviour of the outline cycling performed by
shift-tab varies according to where the cursor is in the text;
it's easiest to learn these behaviours by experimentation.

Outline cycling is a simplification of the full outline mode
behaviour which provides the things one most often wants to
do--additional functionality is available from the Outline menu.

MENU:
=====

A simple menu enables you to generate a template or check the ATF you
are typing.

CHECKING:
=========

After you call the checker you will have two windows; one with
your text, the other with the errors (if any).  Use the key
sequence Ctrl-x-` (control-x then backtick) or C-c-C-n (control-c
then control-n) to jump to the first error in your ATF file, and
then repeat this to step through the errors.

For the ATF documentation see URL `http://oracc.museum.upenn.edu/doc/help/editinginatf/index.html'."
  (interactive "P")
  (let ((on-p atf-mode))
    (kill-all-local-variables)
    (setq atf-mode
	  (if (null arg)
	      (not on-p)
	    (> (prefix-numeric-value arg) 0)))
    (when atf-mode
      (use-local-map atf-mode-map)
      (easy-menu-define atf-mode-menu atf-mode-map "" 
      			(list "ATF" 
			      ["Check ATF" atf-check t]
			      ["Count lemmata" atf-lemcount t]
			      ["Create Template" atf-template t]
			      ["Harvest Notices" atf-harvest-notices t]
			      ["Lemmatize buffer" atf-lemmatize t]
			      ["ATF mode help" describe-mode t]
			      ["Template Help" atf-template-help t]
			      ["Browse ATF Docs" atf-browse-doc t]
			      ))
      (set (make-local-variable 'font-lock-defaults) 
	   '(atf-font-lock-keywords))
      (set (make-local-variable 'font-lock-string-face)
	   nil)
      (if (string= oracc-prefix "slash")
	  (turn-on-cuneitex)
	(turn-on-cuneitex_de))
      (setq major-mode 'atf-mode)
      (setq mode-name "ATF")
      (set-frame-font atf-default-font)
      (atf-otl-mode)
      (run-hooks 'atf-mode-hook))))

(defvar atf-otl-string " ")
(defun atf-otl-mode ()
  (add-hook 'outline-minor-mode-hook
	    '(lambda ()
	       (define-key outline-minor-mode-map [(shift tab)] 'outline-cycle)
	       (setq outline-regexp "\\(&\\|@\\|[\$a-zA-Z0-9]+.\\)")
	       (set-display-table-slot standard-display-table 
				       'selective-display 
				       (string-to-vector atf-otl-string))
	       (setq outline-level
		     '(lambda ()
			(cond ((looking-at "&") 1)
			      ((looking-at "@") 2)
			      ((looking-at "[\$a-zA-Z0-9]") 3)
			      (t 1000))))))
  (outline-minor-mode)
  )

(defun atf-pipes ()
  (interactive)
  (atf-insert-pair "|" "|"))
(defun atf-square ()
  (interactive)
  (atf-insert-pair "[" "]"))
(defun atf-round ()
  (interactive)
  (atf-insert-pair "(" ")"))
(defun atf-curly ()
  (interactive)
  (atf-insert-pair "{" "}"))
(defun atf-angle ()
  (interactive)
  (atf-insert-pair "<" ">"))

(defun atf-insert-pair (o c)
  "Insert an opener-closer pair.  If mark is active, insert
the pair around the region and reset the region to the correct
category.  Otherwise insert the pair at point."
  (interactive)
  (if mark-active
      (atf-insert-pair-region (region-beginning) (region-end) o c)
    (insert o)
    (insert c)
    (backward-char 1)))

(defun atf-insert-pair-region (start end o c)
  "Insert an opener-closer pair around a region."
  (interactive "r")
  (save-excursion
    (goto-char end)
    (insert c)
    (goto-char start)
    (insert o)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                 ATF CHECKING OVER XML-RPC
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun atf-check-post-async(res)
  (let ((inhibit-read-only t))
    (insert
     (atf-rpc-resval (res)))))

(defun atf-check-rpc-async(data name args)
  (interactive)
  (xml-rpc-method-call (concat http-oracc-build "cgi-bin/rpc")
		       'oracc.atf.check
		       `(("data" . ,data)
			 ("atf-file-name" . ,name)
			 ("atf-args" . ,args))))

(defun atf-check-rpc(data name args)
  (interactive)
  (let*
      ((inhibit-read-only t)
       (resp (xml-rpc-method-call (concat http-oracc-build "cgi-bin/rpc")
				  'oracc.atf.check
				  `(("data" . ,data)
				    ("atf-file-name" . ,name)
				    ("atf-args" . ,args)
				    )))
       )
    (insert (nth 2 resp))))

(defun atf-check-async () ; based on 'compile'
  (interactive)
  (save-some-buffers (not compilation-ask-about-save) nil)
  (atf-check-start-async 
   (buffer-substring-no-properties (point-min) (point-max))
; buffer-file-name has the path--we should really either
; use that and then basename it or take care to cut a possible
; <1> etc. off of buffer-name
   (buffer-name)))

(defun atf-check-start-async (data name)
  (setq mode 'compilation-mode)
  (let* ((name-of-mode
	  (if (eq mode t)
	      (prog1 "compilation" (require 'comint))
	    (replace-regexp-in-string "-mode$" "" (symbol-name mode))))
	 (thisdir default-directory)
	 outwin outbuf)
    (with-current-buffer
	(setq outbuf
	      (get-buffer-create
	       (compilation-buffer-name name-of-mode mode nil)))
      (buffer-disable-undo (current-buffer))
      (setq default-directory thisdir)
      (let ((inhibit-read-only t)
	    (default-directory thisdir))
	(cd default-directory)
	(erase-buffer)
	;; Select the desired mode.
	(if (not (eq mode t))
	    (funcall mode)
	  (setq buffer-read-only nil)
	  (with-no-warnings (comint-mode))
	  (compilation-shell-minor-mode))
	(insert "-*- mode: " name-of-mode
		"; default-directory: " (prin1-to-string default-directory)
		" -*-\n"
		(format "%s started at %s\n\n"
			mode-name
			(substring (current-time-string) 0 19))
		"atf-check" "\n")
	(setq thisdir default-directory))
      (set-buffer-modified-p nil))
    ;; If we're already in the compilation buffer, go to the end
    ;; of the buffer, so point will track the compilation output.
    (if (eq outbuf (current-buffer))
	(goto-char (point-max)))
    ;; Pop up the compilation buffer.
    (setq outwin (display-buffer outbuf nil t))
    (with-current-buffer outbuf
      (set (make-local-variable 'compilation-highlight-regexp) t)
      (set-window-start outwin (point-min))
	(or (eq outwin (selected-window))
	    (set-window-point outwin (if compilation-scroll-output
					 (point)
				       (point-min))))
	;; The setup function is called before compilation-set-window-height
	;; so it can set the compilation-window-height buffer locally.
	(compilation-set-window-height outwin)
	  ;; Without async subprocesses, the buffer is not yet
	  ;; fontified, so fontify it now.
	(atf-check-rpc-async data name)
	  (let ((font-lock-verbose nil)) ; shut up font-lock messages
	    (font-lock-fontify-buffer))
	  (set-buffer-modified-p nil)
	  (message "Executing `%s'...done" "atf-check"))
      ;; Now finally cd to where the shell started make/grep/...
      (setq default-directory thisdir)
    (if (buffer-local-value 'compilation-scroll-output outbuf)
	(save-selected-window
	  (select-window outwin)
	  (goto-char (point-max))))
    ;; Make it so the next C-x ` will use this buffer.
    (setq next-error-last-buffer outbuf)))


(defun atf-check ()
  (interactive)
  (atf-run-with-args "cvm"))

(defun atf-harvest-notices ()
  (interactive)
  (atf-run-with-args "cvmh"))

(defun atf-processor (args)
  (interactive "sEnter arguments for ATF processor: ")
  (atf-run-with-args args))

(defun atf-run-with-args (args)
  (save-some-buffers (not compilation-ask-about-save) nil)
  (atf-check-start 
   (buffer-substring-no-properties (point-min) (point-max))
; buffer-file-name has the path--we should really either
; use that and then basename it or take care to cut a possible
; <1> etc. off of buffer-name
   (buffer-name)
   args))

(defun atf-check-start (data name args)
  (setq mode 'compilation-mode)
  (let* ((name-of-mode
	  (if (eq mode t)
	      (prog1 "compilation" (require 'comint))
	    (replace-regexp-in-string "-mode$" "" (symbol-name mode))))
	 (thisdir default-directory)
	 outwin outbuf)
    (with-current-buffer
	(setq outbuf
	      (get-buffer-create
	       (compilation-buffer-name name-of-mode mode nil)))
;      (let ((comp-proc (get-buffer-process (current-buffer))))
;	(if comp-proc
;	    (if (or (not (eq (process-status comp-proc) 'run))
;		    (yes-or-no-p
;		     (format "A %s process is running; kill it? "
;			     name-of-mode)))
;		(condition-case ()
;		    (progn
;		      (interrupt-process comp-proc)
;		      (sit-for 1)
;		      (delete-process comp-proc))
;		  (error nil))
;	      (error "Cannot have two processes in `%s' at once"
;		     (buffer-name)))))
      (buffer-disable-undo (current-buffer))
      ;; first transfer directory from where M-x compile was called
      (setq default-directory thisdir)
      ;; Make compilation buffer read-only.  The filter can still write it.
      ;; Clear out the compilation buffer.
      (let ((inhibit-read-only t)
	    (default-directory thisdir))
	;; Then evaluate a cd command if any, but don't perform it yet, else start-command
	;; would do it again through the shell: (cd "..") AND sh -c "cd ..; make"
	(cd ;(if (string-match "^\\s *cd\\(?:\\s +\\(\\S +?\\)\\)?\\s *[;&\n]" command)
	    ;	(if (match-end 1)
	    ;	    (substitute-env-vars (match-string 1 command))
	    ;	  "~")
	      default-directory)
	(erase-buffer)
	;; Select the desired mode.
	(if (not (eq mode t))
	    (funcall mode)
	  (setq buffer-read-only nil)
	  (with-no-warnings (comint-mode))
	  (compilation-shell-minor-mode))
;	(if highlight-regexp
;	    (set (make-local-variable 'compilation-highlight-regexp)
;		 highlight-regexp))
	;; Output a mode setter, for saving and later reloading this buffer.
	(insert "-*- mode: " name-of-mode
		"; default-directory: " (prin1-to-string default-directory)
		" -*-\n"
		(format "%s started at %s\n\n"
			mode-name
			(substring (current-time-string) 0 19))
		"atf-check" "\n")
	(setq thisdir default-directory))
      (set-buffer-modified-p nil))
    ;; If we're already in the compilation buffer, go to the end
    ;; of the buffer, so point will track the compilation output.
    (if (eq outbuf (current-buffer))
	(goto-char (point-max)))
    ;; Pop up the compilation buffer.
    (setq outwin (display-buffer outbuf nil t))
    (with-current-buffer outbuf
;      (let ((process-environment
;	     (append
;	      compilation-environment
;	      (if (if (boundp 'system-uses-terminfo) ; `if' for compiler warning
;		      system-uses-terminfo)
;		  (list "TERM=dumb" "TERMCAP="
;			(format "COLUMNS=%d" (window-width)))
;		(list "TERM=emacs"
;		      (format "TERMCAP=emacs:co#%d:tc=unknown:"
;			      (window-width))))
;	      ;; Set the EMACS variable, but
;	      ;; don't override users' setting of $EMACS.
;	      (unless (getenv "EMACS") '("EMACS=t"))
;	      (copy-sequence process-environment))))
;	(set (make-local-variable 'compilation-arguments)
;	     (list command mode name-function highlight-regexp))
;	(set (make-local-variable 'revert-buffer-function)
;	     'compilation-revert-buffer)

      (set (make-local-variable 'compilation-highlight-regexp) t)
      (set-window-start outwin (point-min))
	(or (eq outwin (selected-window))
	    (set-window-point outwin (if compilation-scroll-output
					 (point)
				       (point-min))))
	;; The setup function is called before compilation-set-window-height
	;; so it can set the compilation-window-height buffer locally.
;	(if compilation-process-setup-function
;	    (funcall compilation-process-setup-function))
	(compilation-set-window-height outwin)
	;; Start the compilation.
;	(if (fboundp 'start-process)
;	    (let ((proc (if (eq mode t)
;			    (get-buffer-process
;			     (with-no-warnings
;			      (comint-exec outbuf (downcase mode-name)
;					   shell-file-name nil `("-c" ,command))))
;			  (start-process-shell-command (downcase mode-name)
;						       outbuf command))))
;	      ;; Make the buffer's mode line show process state.
;	      (setq mode-line-process '(":%s"))
;	      (set-process-sentinel proc 'compilation-sentinel)
;	      (set-process-filter proc 'compilation-filter)
;	      (set-marker (process-mark proc) (point) outbuf)
;	      (when compilation-disable-input
;                (condition-case nil
;                    (process-send-eof proc)
;                  ;; The process may have exited already.
;                  (error nil)))
;	      (setq compilation-in-progress
;		    (cons proc compilation-in-progress)))
;	  ;; No asynchronous processes available.
;	  (message "Executing `%s'..." command)
;	  ;; Fake modeline display as if `start-process' were run.
;	  (setq mode-line-process ":run")
;	  (force-mode-line-update)
;	  (sit-for 0)			; Force redisplay
;	  (let* ((buffer-read-only nil)	; call-process needs to modify outbuf
;		 (status (call-process shell-file-name nil outbuf nil "-c"
;				       command)))
;	    (cond ((numberp status)
;		   (compilation-handle-exit 'exit status
;					    (if (zerop status)
;						"finished\n"
;					      (format "\
;exited abnormally with code %d\n"
;						      status))))
;		  ((stringp status)
;		   (compilation-handle-exit 'signal status
;					    (concat status "\n")))
;		  (t
;		   (compilation-handle-exit 'bizarre status status)))
	  ;; Without async subprocesses, the buffer is not yet
	  ;; fontified, so fontify it now.
	(atf-check-rpc data name args)
	  (let ((font-lock-verbose nil)) ; shut up font-lock messages
	    (font-lock-fontify-buffer))
	  (set-buffer-modified-p nil)
	  (message "Executing `%s'...done" "atf-check"))
      ;; Now finally cd to where the shell started make/grep/...
      (setq default-directory thisdir)
    (if (buffer-local-value 'compilation-scroll-output outbuf)
	(save-selected-window
	  (select-window outwin)
	  (goto-char (point-max))))
    ;; Make it so the next C-x ` will use this buffer.
    (setq next-error-last-buffer outbuf)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                 ATF TEMPLATE OVER XML-RPC
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun atf-template ()
"An easy way to use the ATF template generator to make ATF templates.

The ATF generator turns template specifications into templates which
give the entire block structure of a text; once you have the template,
you only have to type the lines of transliteration.

A template specification is a series of lines: lines that start
with '&' are ATF &-lines; if the line has an ID starting with P
or Q, the designation is looked up and added; if it has an ID
starting with X, or if it has no ID the template generator tries
to find the P-number for the text based on the rest of the
line (if there is an X-ID, the rest of the line following the '='
sign is used).  If it can't find a P-ID it just outputs the
&-line unchanged.  If multiple P-IDs appear to match the keys in
the &-line, all the matches are output.

If a line starts with '#' it is a comment; the line is passed
through unchanged.

Otherwise, the line is parsed as a list of block and line-number
specifications.  Each specification token is separated by spaces.
The rules for the tokens are simple:

  o = obverse      r = reverse

  le = leftedge    re = rightedge   te = topedge    be = bottomedge

  c<N> = column N (e.g., c1 = column 1; c11 = column 11, etc.)

  N = line number (e.g., 4)    

  N1-N2 = range of line numbers (e.g., 1-30)

The normal prime, exclamation and question-mark flags are allowed
at the end of any token so you can say 1'-30'; in addition, three
special flags are understood which create $-lines instead of
block tags and line numbers:

  ] = broken   # = traces    _ = blank

A special distinction is made in $-specifiers between `cN]' and `Nc]': 
the former generates 
  
  $ column N broken

the latter generates 

  $ N columns broken

The special codes `c-]' and `c+]' generate

  $ start of column broken 

and 

  $ rest of column broken

respectively; the other special flags behave similarly.

Similarly, `o-]', `o+]', `r-]', `r+]', and all the other surface
designators, produce 

  $ start of obverse broken

etc.

It's easiest to learn to use this utility by playing with it; if
you think your template is wrong, or you are really just playing
around to learn, simply use the undo command after creating the
template.  This removes the newly inserted template leaving only
the template specification behind for you to modify.  When you
are satisfied that your template is what you want you can
delete the specification.

Here is an example:

  &P100001
  #a comment
  1-2 3] 1'-3' r c1 1-3 c2#

Generates

  &P100001 = AAS 013
  #atf: lang sux
  #a comment
  1.	
  2.	
  $ 3 lines broken
  1'.	
  2'.	
  3'.	
  @reverse
  @column 1
  1.	
  2.	
  3.	
  $ column 2 traces
"
  (interactive)
  (let ((inhibit-read-only t))
    (insert
     (nth 2
	(xml-rpc-method-call (concat http-oracc-build "cgi-bin/rpc.plx")
			     'oracc.atf.template
			     `(("data" . ,(buffer-substring-no-properties 
					   (point-min) (point-max)))
			       ))))))

(defun atf-template-help ()
  (interactive)
  (describe-function 'atf-template))

(defun atf-lemcount ()
  "Send a buffer's contents to the ATF processor for lemmatization."
  (interactive)
  (let* ((bufstr (buffer-substring-no-properties 
		  (point-min) (point-max)))
	 (name (buffer-name))
	 (resp (xml-rpc-method-call (concat http-oracc-build "cgi-bin/rpc.plx")
				    'oracc.atf.lemcount
				    `(("data" . ,bufstr)
				      ("atf-file-name" . ,name)))))
    (if (atf-rpc-errorp resp)
	(let ((msg (atf-rpc-errmsg resp)))
	  (x-popup-dialog (frame-selected-window) `(,msg ("OK" . t)) t))
      (save-excursion
	(set-buffer (find-file "*lemcount*"))
	(erase-buffer)
	(insert (atf-rpc-resval resp))))))

(defun atf-lemmatize ()
  "Send a buffer's contents to the ATF processor for lemmatization."
  (interactive)
  (let* ((bufstr (buffer-substring-no-properties 
		  (point-min) (point-max)))
	 (name (buffer-name))
	 (resp (xml-rpc-method-call (concat http-oracc-build "cgi-bin/rpc.plx")
				    'oracc.atf.lemmatize
				    `(("data" . ,bufstr)
				      ("atf-file-name" . ,name)))))
    (if (atf-rpc-errorp resp)
	(let ((msg (atf-rpc-errmsg resp)))
	  (x-popup-dialog (frame-selected-window) `(,msg ("OK" . t)) t))
      (let
	  ((bak-fname (concat (file-name-sans-extension buffer-file-name) 
			      ".bak"))
	   (xpoint (point)))
	(save-excursion
	  (set-buffer (find-file bak-fname))
	  (erase-buffer)
	  (insert bufstr)
	  (save-buffer)
	  (kill-buffer nil))
	(erase-buffer)
	(insert (atf-rpc-resval resp))
;	  (set-window-point xpoint)
	(goto-char xpoint)))))

(defun atf-rpc-errorp (res)
  (if (string= (nth 0 res) nil)
      nil
      t))

(defun atf-rpc-errmsg (res)
  (nth 1 res))

(defun atf-rpc-resval (res)
  (nth 2 res))

(defun atf-browse-doc ()
  (interactive)
  (browse-url "http://oracc.museum.upenn.edu/doc/help/editinginatf/index.html"))

(defun atf-version ()
  "Display the release-version of the ATF mode you are using"
  (interactive)
  (message "ATF mode version 2022-11-04")
)

(provide 'atf-mode)
