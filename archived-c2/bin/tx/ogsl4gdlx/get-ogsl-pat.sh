#!/bin/sh
tr -d '₀-₉ₓ' <ogsl.lst \
    | sed 's/[A-ZṢŠṬḪŊ]\+/A/g' \
    | sed 's/[a-zṣšṭḫŋʾ]\+/a/g' \
    | sed 's/[0-9]\+/1/g' \
	  >ogsl.pat
