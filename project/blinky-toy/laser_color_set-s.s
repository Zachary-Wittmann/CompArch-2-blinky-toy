	.arch msp430g2553
	.p2align 1,0
	.text

	.equ RED, 1
	.equ GREEN, 64

	.global laser_color_set
	.extern laser_color

laser_color_set:	
	cmp #RED, &laser_color
	jc cg_red
	mov #GREEN, &laser_color
	jmp done
cg_red:	mov #RED, &laser_color
done:	pop r0
