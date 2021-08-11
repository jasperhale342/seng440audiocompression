	.arch armv4t
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 18, 4
	.file	"audio_compression_hardware_support.c"
	.global	__aeabi_i2d
	.global	__aeabi_ddiv
	.section	.rodata
	.align	2
.LC0:
	.ascii	"FILE COMPRESSED. time: %f\012\000"
	.text
	.align	2
	.global	encode_data
	.type	encode_data, %function
encode_data:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 48
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, fp, lr}
	add	fp, sp, #16
	sub	sp, sp, #52
	str	r0, [fp, #-56]
	str	r1, [fp, #-60]
	str	r2, [fp, #-64]
	bl	clock
	mov	r3, r0
	str	r3, [fp, #-44]
	mov	r3, #0
	str	r3, [fp, #-40]
	b	.L2
.L3:
	ldr	r3, [fp, #-40]
	mov	r3, r3, asr #1
	str	r3, [fp, #-36]
	ldr	r2, [fp, #-40]
	ldr	r3, [fp, #-56]
	add	r3, r3, r2
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r1, r3
	ldr	r3, [fp, #-40]
	add	r2, r3, #1
	ldr	r3, [fp, #-56]
	add	r3, r3, r2
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #16
	mov	r2, r1	@ movhi
	orr	r3, r2, r3
	strh	r3, [fp, #-50]	@ movhi
	ldrsh	r3, [fp, #-50]
	mov	r0, r3
	bl	get_sign
	mov	r3, r0
	strb	r3, [fp, #-47]
	ldrsh	r3, [fp, #-50]
	mov	r0, r3
	bl	get_magnitude
	mov	r3, r0
	strh	r3, [fp, #-46]	@ movhi
	ldr	r2, [fp, #-36]
	ldr	r3, [fp, #-60]
	add	r4, r3, r2
	ldrh	r3, [fp, #-46]
	ldrb	r2, [fp, #-47]	@ zero_extendqisi2
	mov	r0, r3
	mov	r1, r2
	bl	codeword_compression
	mov	r3, r0
	strb	r3, [r4, #0]
	ldr	r3, [fp, #-40]
	add	r3, r3, #2
	str	r3, [fp, #-40]
.L2:
	ldr	r3, [fp, #-64]
	sub	r2, r3, #1
	ldr	r3, [fp, #-40]
	cmp	r2, r3
	bgt	.L3
	bl	clock
	mov	r3, r0
	str	r3, [fp, #-32]
	ldr	r2, [fp, #-32]
	ldr	r3, [fp, #-44]
	rsb	r3, r3, r2
	mov	r0, r3
	bl	__aeabi_i2d
	mov	r3, r0
	mov	r4, r1
	mov	r5, #0
	mov	r6, #1090519040
	add	r6, r6, #3047424
	add	r6, r6, #1152
	mov	r0, r3
	mov	r1, r4
	mov	r2, r5
	mov	r3, r6
	bl	__aeabi_ddiv
	mov	r3, r0
	mov	r4, r1
	str	r3, [fp, #-28]
	str	r4, [fp, #-24]
	ldr	r0, .L5
	sub	r3, fp, #28
	ldmia	r3, {r2-r3}
	bl	printf
	sub	sp, fp, #16
	ldmfd	sp!, {r4, r5, r6, fp, lr}
	bx	lr
.L6:
	.align	2
.L5:
	.word	.LC0
	.size	encode_data, .-encode_data
	.align	2
	.global	decode_data
	.type	decode_data, %function
decode_data:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #24
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	str	r2, [fp, #-24]
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L8
.L9:
	ldr	r2, [fp, #-8]
	ldr	r3, [fp, #-16]
	add	r3, r3, r2
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	eor	r3, r3, #85
	strb	r3, [fp, #-9]
	ldrb	r3, [fp, #-9]	@ zero_extendqisi2
	mov	r0, r3
	bl	codeword_decompression
	mov	r3, r0
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #16
	strh	r3, [fp, #-12]	@ movhi
	ldr	r3, [fp, #-8]
	mov	r2, r3, asl #1
	ldr	r3, [fp, #-20]
	add	r2, r3, r2
	ldrh	r3, [fp, #-12]	@ movhi
	strh	r3, [r2, #0]	@ movhi
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L8:
	ldr	r2, [fp, #-8]
	ldr	r3, [fp, #-24]
	cmp	r2, r3
	blt	.L9
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
	.size	decode_data, .-decode_data
	.section	.rodata
	.align	2
.LC1:
	.ascii	"input_compressed.wav\000"
	.align	2
.LC2:
	.ascii	"wb\000"
	.text
	.align	2
	.global	write_encoded_data
	.type	write_encoded_data, %function
write_encoded_data:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #24
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	str	r2, [fp, #-24]
	ldr	r0, .L15
	ldr	r1, .L15+4
	bl	fopen
	mov	r3, r0
	str	r3, [fp, #-12]
	ldr	r0, [fp, #-20]
	ldr	r1, [fp, #-12]
	bl	write_header_encoded_file
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L12
.L13:
	ldr	r2, [fp, #-8]
	ldr	r3, [fp, #-16]
	add	r3, r3, r2
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-12]
	bl	fwrite
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L12:
	ldr	r2, [fp, #-8]
	ldr	r3, [fp, #-24]
	cmp	r2, r3
	blt	.L13
	ldr	r0, [fp, #-12]
	bl	fclose
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L16:
	.align	2
.L15:
	.word	.LC1
	.word	.LC2
	.size	write_encoded_data, .-write_encoded_data
	.section	.rodata
	.align	2
.LC3:
	.ascii	"input_decompressed.wav\000"
	.text
	.align	2
	.global	write_decoded_data
	.type	write_decoded_data, %function
write_decoded_data:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #24
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	str	r2, [fp, #-24]
	ldr	r0, .L21
	ldr	r1, .L21+4
	bl	fopen
	mov	r3, r0
	str	r3, [fp, #-12]
	ldr	r0, [fp, #-20]
	ldr	r1, [fp, #-12]
	bl	write_header_decoded_file
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L18
.L19:
	ldr	r3, [fp, #-8]
	mov	r2, r3, asl #1
	ldr	r3, [fp, #-16]
	add	r3, r3, r2
	mov	r0, r3
	mov	r1, #2
	mov	r2, #1
	ldr	r3, [fp, #-12]
	bl	fwrite
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L18:
	ldr	r2, [fp, #-8]
	ldr	r3, [fp, #-24]
	cmp	r2, r3
	blt	.L19
	ldr	r0, [fp, #-12]
	bl	fclose
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L22:
	.align	2
.L21:
	.word	.LC3
	.word	.LC2
	.size	write_decoded_data, .-write_decoded_data
	.align	2
	.global	read_data
	.type	read_data, %function
read_data:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #32
	str	r0, [fp, #-32]
	str	r1, [fp, #-36]
	ldr	r3, [fp, #-36]
	ldr	r3, [r3, #12]
	sub	r3, r3, #36
	str	r3, [fp, #-24]
	ldr	r3, [fp, #-24]
	mov	r3, r3, asr #1
	str	r3, [fp, #-20]
	ldr	r3, [fp, #-24]
	mov	r0, r3
	bl	malloc
	mov	r3, r0
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-24]
	ldr	r0, [fp, #-16]
	mov	r1, r3
	mov	r2, #1
	ldr	r3, [fp, #-32]
	bl	fread
	ldr	r3, [fp, #-24]
	mov	r0, r3
	bl	malloc
	mov	r3, r0
	str	r3, [fp, #-12]
	ldr	r3, [fp, #-20]
	mov	r3, r3, asl #1
	mov	r0, r3
	bl	malloc
	mov	r3, r0
	str	r3, [fp, #-8]
	ldr	r0, [fp, #-16]
	ldr	r1, [fp, #-12]
	ldr	r2, [fp, #-24]
	bl	encode_data
	ldr	r0, [fp, #-12]
	ldr	r1, [fp, #-36]
	ldr	r2, [fp, #-24]
	bl	write_encoded_data
	ldr	r0, [fp, #-12]
	ldr	r1, [fp, #-8]
	ldr	r2, [fp, #-20]
	bl	decode_data
	ldr	r0, [fp, #-8]
	ldr	r1, [fp, #-32]
	ldr	r2, [fp, #-20]
	bl	write_decoded_data
	ldr	r0, [fp, #-16]
	bl	free
	ldr	r0, [fp, #-12]
	bl	free
	ldr	r0, [fp, #-8]
	bl	free
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
	.size	read_data, .-read_data
	.align	2
	.global	write_header_decoded_file
	.type	write_header_decoded_file, %function
write_header_decoded_file:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 56
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #56
	str	r0, [fp, #-56]
	str	r1, [fp, #-60]
	ldr	r0, [fp, #-56]
	mov	r1, #0
	mov	r2, #0
	bl	fseek
	sub	r3, fp, #48
	mov	r0, r3
	mov	r1, #44
	mov	r2, #1
	ldr	r3, [fp, #-56]
	bl	fread
	sub	r3, fp, #48
	mov	r0, r3
	mov	r1, #44
	mov	r2, #1
	ldr	r3, [fp, #-60]
	bl	fwrite
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
	.size	write_header_decoded_file, .-write_header_decoded_file
	.align	2
	.global	write_header_encoded_file
	.type	write_header_encoded_file, %function
write_header_encoded_file:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #24
	str	r0, [fp, #-24]
	str	r1, [fp, #-28]
	ldr	r0, [fp, #-28]
	mov	r1, #0
	mov	r2, #0
	bl	fseek
	ldr	r3, [fp, #-24]
	add	r3, r3, #8
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	ldr	r3, [fp, #-24]
	ldr	r3, [r3, #12]
	sub	r3, r3, #36
	mov	r3, r3, lsr #1
	add	r3, r3, #36
	str	r3, [fp, #-12]
	ldr	r3, [fp, #-12]
	and	r3, r3, #255
	strb	r3, [fp, #-15]
	ldr	r3, [fp, #-12]
	mov	r3, r3, lsr #8
	and	r3, r3, #255
	strb	r3, [fp, #-16]
	ldr	r3, [fp, #-12]
	mov	r3, r3, lsr #16
	and	r3, r3, #255
	strb	r3, [fp, #-17]
	ldr	r3, [fp, #-12]
	mov	r3, r3, lsr #24
	and	r3, r3, #255
	strb	r3, [fp, #-18]
	sub	r3, fp, #18
	add	r3, r3, #3
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	add	r3, r3, #2
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	add	r3, r3, #1
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	mov	r0, r3
	mov	r1, #0
	mov	r2, #4
	bl	memset
	ldr	r3, [fp, #-24]
	add	r3, r3, #16
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	ldr	r3, [fp, #-24]
	add	r3, r3, #20
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	ldr	r3, [fp, #-24]
	add	r3, r3, #24
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	mov	r3, #6
	strb	r3, [fp, #-14]
	mov	r3, #0
	strb	r3, [fp, #-13]
	sub	r3, fp, #14
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #14
	add	r3, r3, #1
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #14
	mov	r0, r3
	mov	r1, #0
	mov	r2, #2
	bl	memset
	ldr	r3, [fp, #-24]
	add	r3, r3, #30
	mov	r0, r3
	mov	r1, #2
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	ldr	r3, [fp, #-24]
	add	r3, r3, #32
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	ldr	r3, [fp, #-24]
	ldr	r3, [r3, #36]
	and	r3, r3, #255
	strb	r3, [fp, #-15]
	ldr	r3, [fp, #-24]
	ldr	r3, [r3, #36]
	mov	r3, r3, lsr #8
	and	r3, r3, #255
	strb	r3, [fp, #-16]
	ldr	r3, [fp, #-24]
	ldr	r3, [r3, #36]
	mov	r3, r3, lsr #16
	and	r3, r3, #255
	strb	r3, [fp, #-17]
	ldr	r3, [fp, #-24]
	ldr	r3, [r3, #36]
	mov	r3, r3, lsr #24
	and	r3, r3, #255
	strb	r3, [fp, #-18]
	sub	r3, fp, #18
	add	r3, r3, #3
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	add	r3, r3, #2
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	add	r3, r3, #1
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	mov	r0, r3
	mov	r1, #0
	mov	r2, #4
	bl	memset
	ldr	r3, [fp, #-24]
	ldr	r3, [r3, #40]
	and	r3, r3, #255
	strb	r3, [fp, #-13]
	ldr	r3, [fp, #-24]
	ldr	r3, [r3, #40]
	mov	r3, r3, lsr #8
	and	r3, r3, #255
	strb	r3, [fp, #-14]
	sub	r3, fp, #14
	add	r3, r3, #1
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #14
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #14
	mov	r0, r3
	mov	r1, #0
	mov	r2, #2
	bl	memset
	ldr	r3, [fp, #-24]
	ldrh	r3, [r3, #44]
	and	r3, r3, #255
	strb	r3, [fp, #-14]
	mov	r3, #0
	strb	r3, [fp, #-13]
	sub	r3, fp, #14
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #14
	add	r3, r3, #1
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #14
	mov	r0, r3
	mov	r1, #0
	mov	r2, #2
	bl	memset
	ldr	r3, [fp, #-24]
	add	r3, r3, #46
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	ldr	r3, [fp, #-12]
	sub	r3, r3, #36
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-8]
	and	r3, r3, #255
	strb	r3, [fp, #-15]
	ldr	r3, [fp, #-8]
	mov	r3, r3, lsr #8
	and	r3, r3, #255
	strb	r3, [fp, #-16]
	ldr	r3, [fp, #-8]
	mov	r3, r3, lsr #16
	and	r3, r3, #255
	strb	r3, [fp, #-17]
	ldr	r3, [fp, #-8]
	mov	r3, r3, lsr #24
	and	r3, r3, #255
	strb	r3, [fp, #-18]
	sub	r3, fp, #18
	add	r3, r3, #3
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	add	r3, r3, #2
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	add	r3, r3, #1
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	mov	r0, r3
	mov	r1, #1
	mov	r2, #1
	ldr	r3, [fp, #-28]
	bl	fwrite
	sub	r3, fp, #18
	mov	r0, r3
	mov	r1, #0
	mov	r2, #4
	bl	memset
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
	.size	write_header_encoded_file, .-write_header_encoded_file
	.align	2
	.global	codeword_decompression
	.type	codeword_decompression, %function
codeword_decompression:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #20
	mov	r3, r0
	strb	r3, [fp, #-13]
	mov	r3, #0
	str	r3, [fp, #-8]
	ldrb	r2, [fp, #-13]
	ldr	r3, [fp, #-8]
#APP
@ 210 "audio_compression_hardware_support.c" 1
	alaw   r3, r2, r3
@ 0 "" 2
	str	r3, [fp, #-12]
	ldr	r3, [fp, #-12]
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #16
	mov	r3, r3, asl #16
	mov	r3, r3, asr #16
	mov	r0, r3
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
	.size	codeword_decompression, .-codeword_decompression
	.section	.rodata
	.align	2
.LC4:
	.ascii	"START OF GETTING WAVE HEADER INFO\000"
	.align	2
.LC5:
	.ascii	"GETTING RIFF\000"
	.align	2
.LC6:
	.ascii	"GETTING FILE SIZE\000"
	.align	2
.LC7:
	.ascii	"File size: %d\012\000"
	.align	2
.LC8:
	.ascii	"GETTING FILE TYPE HEADER\000"
	.align	2
.LC9:
	.ascii	"GETTING CHUNK MARKER\000"
	.align	2
.LC10:
	.ascii	"GETTING FORMAT DATA LENGTH\000"
	.align	2
.LC11:
	.ascii	"GETTING FORMAT TYPE\000"
	.align	2
.LC12:
	.ascii	"Format Type: %d\012\000"
	.align	2
.LC13:
	.ascii	"GETTING NUMBER OF CHANNELS\000"
	.align	2
.LC14:
	.ascii	"GETTING SAMPLE RATE\000"
	.align	2
.LC15:
	.ascii	"Sample rate : %d\012\000"
	.align	2
.LC16:
	.ascii	"GETTING BYTERATE\000"
	.align	2
.LC17:
	.ascii	"GETTING block align\000"
	.align	2
.LC18:
	.ascii	"GETTING BITS PER SAMPLE\000"
	.align	2
.LC19:
	.ascii	"GETTING DATA HEADER\000"
	.align	2
.LC20:
	.ascii	"GETTING SIZE OF DATA SECTION\000"
	.align	2
.LC21:
	.ascii	"FINISHED GETTING WAV HEADER INFO\000"
	.global	__aeabi_uidiv
	.text
	.align	2
	.global	get_wave_header_info
	.type	get_wave_header_info, %function
get_wave_header_info:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #16
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	ldr	r0, .L33
	bl	puts
	ldr	r0, .L33+4
	bl	puts
	ldr	r3, [fp, #-20]
	add	r3, r3, #8
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldr	r0, .L33+8
	bl	puts
	sub	r3, fp, #8
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldrb	r3, [fp, #-8]	@ zero_extendqisi2
	mov	r2, r3
	ldrb	r3, [fp, #-7]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	orr	r2, r2, r3
	ldrb	r3, [fp, #-6]	@ zero_extendqisi2
	mov	r3, r3, asl #16
	orr	r2, r2, r3
	ldrb	r3, [fp, #-5]	@ zero_extendqisi2
	mov	r3, r3, asl #24
	orr	r3, r2, r3
	mov	r2, r3
	ldr	r3, [fp, #-20]
	str	r2, [r3, #12]
	sub	r3, fp, #8
	mov	r0, r3
	mov	r1, #0
	mov	r2, #4
	bl	memset
	ldr	r3, [fp, #-20]
	ldr	r3, [r3, #12]
	ldr	r0, .L33+12
	mov	r1, r3
	bl	printf
	ldr	r0, .L33+16
	bl	puts
	ldr	r3, [fp, #-20]
	add	r3, r3, #16
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldr	r0, .L33+20
	bl	puts
	ldr	r3, [fp, #-20]
	add	r3, r3, #20
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldr	r0, .L33+24
	bl	puts
	sub	r3, fp, #8
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldrb	r3, [fp, #-8]	@ zero_extendqisi2
	mov	r2, r3
	ldrb	r3, [fp, #-7]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	orr	r2, r2, r3
	ldrb	r3, [fp, #-6]	@ zero_extendqisi2
	mov	r3, r3, asl #16
	orr	r2, r2, r3
	ldrb	r3, [fp, #-5]	@ zero_extendqisi2
	mov	r3, r3, asl #24
	orr	r3, r2, r3
	mov	r2, r3
	ldr	r3, [fp, #-20]
	str	r2, [r3, #24]
	sub	r3, fp, #8
	mov	r0, r3
	mov	r1, #0
	mov	r2, #4
	bl	memset
	ldr	r0, .L33+28
	bl	puts
	sub	r3, fp, #10
	mov	r0, r3
	mov	r1, #2
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldrb	r3, [fp, #-10]	@ zero_extendqisi2
	mov	r2, r3
	ldrb	r3, [fp, #-9]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #16
	orr	r3, r2, r3
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #16
	mov	r3, r3, asl #16
	mov	r2, r3, lsr #16
	ldr	r3, [fp, #-20]
	strh	r2, [r3, #28]	@ movhi
	ldr	r3, [fp, #-20]
	ldrh	r3, [r3, #28]
	ldr	r0, .L33+32
	mov	r1, r3
	bl	printf
	sub	r3, fp, #10
	mov	r0, r3
	mov	r1, #0
	mov	r2, #2
	bl	memset
	ldr	r0, .L33+36
	bl	puts
	sub	r3, fp, #10
	mov	r0, r3
	mov	r1, #2
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldrb	r3, [fp, #-10]	@ zero_extendqisi2
	mov	r2, r3
	ldrb	r3, [fp, #-9]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #16
	orr	r3, r2, r3
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #16
	mov	r3, r3, asl #16
	mov	r2, r3, lsr #16
	ldr	r3, [fp, #-20]
	strh	r2, [r3, #30]	@ movhi
	sub	r3, fp, #10
	mov	r0, r3
	mov	r1, #0
	mov	r2, #2
	bl	memset
	ldr	r0, .L33+40
	bl	puts
	sub	r3, fp, #8
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldrb	r3, [fp, #-8]	@ zero_extendqisi2
	mov	r2, r3
	ldrb	r3, [fp, #-7]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	orr	r2, r2, r3
	ldrb	r3, [fp, #-6]	@ zero_extendqisi2
	mov	r3, r3, asl #16
	orr	r2, r2, r3
	ldrb	r3, [fp, #-5]	@ zero_extendqisi2
	mov	r3, r3, asl #24
	orr	r3, r2, r3
	mov	r2, r3
	ldr	r3, [fp, #-20]
	str	r2, [r3, #32]
	sub	r3, fp, #8
	mov	r0, r3
	mov	r1, #0
	mov	r2, #4
	bl	memset
	ldr	r3, [fp, #-20]
	ldr	r3, [r3, #32]
	ldr	r0, .L33+44
	mov	r1, r3
	bl	printf
	ldr	r0, .L33+48
	bl	puts
	sub	r3, fp, #8
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldrb	r3, [fp, #-8]	@ zero_extendqisi2
	mov	r2, r3
	ldrb	r3, [fp, #-7]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	orr	r2, r2, r3
	ldrb	r3, [fp, #-6]	@ zero_extendqisi2
	mov	r3, r3, asl #16
	orr	r2, r2, r3
	ldrb	r3, [fp, #-5]	@ zero_extendqisi2
	mov	r3, r3, asl #24
	orr	r3, r2, r3
	mov	r2, r3
	ldr	r3, [fp, #-20]
	str	r2, [r3, #36]
	sub	r3, fp, #8
	mov	r0, r3
	mov	r1, #0
	mov	r2, #4
	bl	memset
	ldr	r0, .L33+52
	bl	puts
	sub	r3, fp, #10
	mov	r0, r3
	mov	r1, #2
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldrb	r3, [fp, #-10]	@ zero_extendqisi2
	mov	r2, r3
	ldrb	r3, [fp, #-9]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	orr	r3, r2, r3
	mov	r2, r3
	ldr	r3, [fp, #-20]
	str	r2, [r3, #40]
	sub	r3, fp, #10
	mov	r0, r3
	mov	r1, #0
	mov	r2, #2
	bl	memset
	ldr	r0, .L33+56
	bl	puts
	sub	r3, fp, #10
	mov	r0, r3
	mov	r1, #2
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldrb	r3, [fp, #-10]	@ zero_extendqisi2
	mov	r2, r3
	ldrb	r3, [fp, #-9]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #16
	orr	r3, r2, r3
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #16
	mov	r3, r3, asl #16
	mov	r2, r3, lsr #16
	ldr	r3, [fp, #-20]
	strh	r2, [r3, #44]	@ movhi
	sub	r3, fp, #10
	mov	r0, r3
	mov	r1, #0
	mov	r2, #2
	bl	memset
	ldr	r0, .L33+60
	bl	puts
	ldr	r3, [fp, #-20]
	add	r3, r3, #46
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldr	r0, .L33+64
	bl	puts
	sub	r3, fp, #8
	mov	r0, r3
	mov	r1, #4
	mov	r2, #1
	ldr	r3, [fp, #-16]
	bl	fread
	ldrb	r3, [fp, #-8]	@ zero_extendqisi2
	mov	r2, r3
	ldrb	r3, [fp, #-7]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	orr	r2, r2, r3
	ldrb	r3, [fp, #-6]	@ zero_extendqisi2
	mov	r3, r3, asl #16
	orr	r2, r2, r3
	ldrb	r3, [fp, #-5]	@ zero_extendqisi2
	mov	r3, r3, asl #24
	orr	r3, r2, r3
	mov	r2, r3
	ldr	r3, [fp, #-20]
	str	r2, [r3, #52]
	sub	r3, fp, #8
	mov	r0, r3
	mov	r1, #0
	mov	r2, #4
	bl	memset
	ldr	r0, .L33+68
	bl	puts
	ldr	r3, [fp, #-20]
	ldr	r3, [r3, #52]
	mov	r1, r3, asl #3
	ldr	r3, [fp, #-20]
	ldrh	r3, [r3, #30]
	mov	r2, r3
	ldr	r3, [fp, #-20]
	ldrh	r3, [r3, #44]
	mul	r3, r2, r3
	mov	r0, r1
	mov	r1, r3
	bl	__aeabi_uidiv
	mov	r3, r0
	mov	r2, r3
	ldr	r3, [fp, #-20]
	str	r2, [r3, #0]
	ldr	r3, [fp, #-20]
	ldrh	r3, [r3, #30]
	mov	r2, r3
	ldr	r3, [fp, #-20]
	ldrh	r3, [r3, #44]
	mul	r3, r2, r3
	add	r2, r3, #7
	cmp	r3, #0
	movlt	r3, r2
	mov	r3, r3, asr #3
	mov	r2, r3
	ldr	r3, [fp, #-20]
	str	r2, [r3, #4]
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L34:
	.align	2
.L33:
	.word	.LC4
	.word	.LC5
	.word	.LC6
	.word	.LC7
	.word	.LC8
	.word	.LC9
	.word	.LC10
	.word	.LC11
	.word	.LC12
	.word	.LC13
	.word	.LC14
	.word	.LC15
	.word	.LC16
	.word	.LC17
	.word	.LC18
	.word	.LC19
	.word	.LC20
	.word	.LC21
	.size	get_wave_header_info, .-get_wave_header_info
	.align	2
	.global	get_magnitude
	.type	get_magnitude, %function
get_magnitude:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	mov	r3, r0
	strh	r3, [fp, #-6]	@ movhi
	ldrsh	r3, [fp, #-6]
	cmp	r3, #0
	bge	.L36
	ldrh	r3, [fp, #-6]	@ movhi
	rsb	r3, r3, #0
	strh	r3, [fp, #-6]	@ movhi
.L36:
	ldrh	r3, [fp, #-6]
	mov	r0, r3
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
	.size	get_magnitude, .-get_magnitude
	.align	2
	.global	get_sign
	.type	get_sign, %function
get_sign:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	mov	r3, r0
	strh	r3, [fp, #-6]	@ movhi
	ldrsh	r3, [fp, #-6]
	cmp	r3, #0
	blt	.L39
	mov	r3, #1
	str	r3, [fp, #-12]
	b	.L40
.L39:
	mov	r3, #0
	str	r3, [fp, #-12]
.L40:
	ldr	r3, [fp, #-12]
	mov	r0, r3
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
	.size	get_sign, .-get_sign
	.align	2
	.global	codeword_compression
	.type	codeword_compression, %function
codeword_compression:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #20
	mov	r3, r0
	mov	r2, r1
	strh	r3, [fp, #-14]	@ movhi
	mov	r3, r2
	strb	r3, [fp, #-15]
	ldrb	r3, [fp, #-15]
	mov	r3, r3, asl #7
	strb	r3, [fp, #-9]
	mov	r3, #1
	str	r3, [fp, #-8]
	ldrh	r2, [fp, #-14]	@ movhi
	ldr	r3, [fp, #-8]
#APP
@ 316 "audio_compression_hardware_support.c" 1
	alaw   r3, r2, r3
@ 0 "" 2
	strb	r3, [fp, #-9]
	ldrb	r3, [fp, #-9]	@ zero_extendqisi2
	mov	r0, r3
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
	.size	codeword_compression, .-codeword_compression
	.section	.rodata
	.align	2
.LC22:
	.ascii	"rb\000"
	.align	2
.LC23:
	.ascii	"no file to compress\000"
	.align	2
.LC24:
	.ascii	"File recieved\000"
	.align	2
.LC25:
	.ascii	"making wav_info structure\000"
	.align	2
.LC26:
	.ascii	"finished writing header to ouput file\000"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #24
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	ldr	r3, [fp, #-20]
	add	r3, r3, #4
	ldr	r3, [r3, #0]
	mov	r0, r3
	ldr	r1, .L47
	bl	fopen
	mov	r3, r0
	str	r3, [fp, #-12]
	ldr	r3, [fp, #-12]
	cmp	r3, #0
	bne	.L45
	ldr	r0, .L47+4
	bl	puts
	mov	r0, #1
	bl	exit
.L45:
	ldr	r0, .L47+8
	bl	puts
	ldr	r0, .L47+12
	bl	puts
	mov	r0, #56
	bl	malloc
	mov	r3, r0
	str	r3, [fp, #-8]
	ldr	r0, [fp, #-12]
	ldr	r1, [fp, #-8]
	bl	get_wave_header_info
	ldr	r0, .L47+16
	bl	puts
	ldr	r0, [fp, #-12]
	ldr	r1, [fp, #-8]
	bl	read_data
	ldr	r0, [fp, #-8]
	bl	free
	ldr	r0, [fp, #-12]
	bl	fclose
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L48:
	.align	2
.L47:
	.word	.LC22
	.word	.LC23
	.word	.LC24
	.word	.LC25
	.word	.LC26
	.size	main, .-main
	.ident	"GCC: (Sourcery G++ Lite 2008q3-72) 4.3.2"
	.section	.note.GNU-stack,"",%progbits
