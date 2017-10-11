;; ORACC version 1.1
;; Cuneitex.el is to be used in combination with atf-mode.el. In atf-mode it uses the back slash (\) as prefix
;; key for inserting special characters such as š, ŋ, or index numbers (in -ŋu₁₀), as documented
;; in http://oracc.museum.upenn.edu/doc/help/usingemacs/emacsforatf/.
;;
;; Use cuneitex_de.el and atf-mode_de.el for usage on German and other keyboards where the back slash is not readily available.
;; In atf-mode_de the comma (,) is used as prefix key - otherwise it works exactly the same as input in regular atf-mode.
;; 
;; Additonal key sequences that are available in the German mode:
;; ,l = [          (this is an l as in left oder links)
;; ,r = ]
;; ,( = ⸢
;; ,) = ⸣
;; ,at = @


;; If your project is on build-oracc use the first oracc-build definition,
;; otherwise use the second.  Use a semi-colon at the start of the line to 
;; comment out the definition you don't need.

(setq oracc-build "build-oracc.museum.upenn.edu")
; (setq oracc-build "oracc.ub.uni-muenchen.de")

;; If you prefer to use the slash (virgule, /) as the dead key for the Oracc
;; Emacs keyboard, use the first definition.  Otherwise use the second to make comma 
;; your prefix key--this is more user-friendly if you are using a German keyboard.  
;; Use a semi-colon at the start of the line to comment out the definition you 
;; don't need.

(setq oracc-prefix "slash")
; (setq oracc-prefix "comma")

(load-library "~/cuneitex.el")
(load-library "~/cuneitex_de.el")
(load-library "~/outline-magic.el")
(load-library "~/xml-rpc.el")
(load-library "~/atf-mode.el")
(load-library "~/cbd-mode.el")
(load-library "~/oracc-project.el")
(load-library "~/oracc-defaults.el")

