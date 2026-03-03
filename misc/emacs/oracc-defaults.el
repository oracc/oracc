;; ORACC version 1.1

(defun turn-on-cuneitex ()
  (set-input-method "Cuneiform"))
(defun turn-on-cuneitex_de ()
  (set-input-method "Cuneiform_de"))

(defcustom atf-default-font nil
  "The font used in ATF and CBD modes.")

(when (boundp 'aquamacs-version)
  (setq atf-default-font 
	"-apple-dejavu sans mono-medium-r-normal--14-*-*-*-m-*-iso10646-1")
  (if mac-input-method-mode
      (mac-input-method-mode))
  (if aquamacs-styles-mode
      (aquamacs-styles-mode)))

(when (eq window-system 'w32)
  (setq atf-default-font
	"-outline-Dejavu Sans Mono-normal-r-normal-normal-12-*-*-*-c-*-iso10646-1"))

(when (eq window-system 'x)
  (setq atf-default-font
	"-*-DejaVu Sans Mono-normal-r-normal-*-*-*-*-*-m-0-*-*"))

(set-language-environment "utf-8")
(if atf-default-font
    (set-default-font atf-default-font))

(defun reset-atf-default-font (font-name)
  (setq atf-default-font font-name)
  (set-default-font atf-default-font)
  (set-frame-font atf-default-font))
