	.arch msp430g2553
	.p2align 1,0
	.text

	.equ RED, 1
	.equ GREEN, 64

	.global laser_color_set
	.extern P1OUT
	
laser_color_set:
	cmp #RED, r12
	jc cg_red
	bis #GREEN, &P1OUT
	and #~RED, &P1OUT
	jmp done
cg_red:	bis #RED, &P1OUT
	and #~GREEN, &P1OUT
done:	pop r0
