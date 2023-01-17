; run: `emacs -batch source.c -l $PWD/format-code.el' to format code
(setq c-basic-offset 4
      c-default-style "k&r"
      tab-width 8
      indent-tabs-mode t)
; hint: use C-c C-s on the mis-indented line to discover its syntactic element
; try to match original style
(c-set-offset 'arglist-cont-nonempty 'c-lineup-dont-change)
(c-set-offset 'arglist-close '+)
(c-set-offset 'comment-intro 'c-lineup-dont-change)
(c-set-offset 'defun-block-intro '++)
(c-set-offset 'inclass '++)
(c-set-offset 'inextern-lang 0)
(c-set-offset 'knr-argdecl-intro '++)
(c-set-offset 'label '*)
(c-set-offset 'substatement-open 0)
(indent-region (point-min) (point-max) nil)
(save-buffer)
