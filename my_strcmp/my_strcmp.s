section .text

global my_strcmp

my_strcmp:
    vmovdqa ymm0, [rdi]
    vmovdqa ymm1, [rsi]

    vpcmpeqb ymm2, ymm0, ymm1
    vpmovmskb eax, ymm2

    not eax

    ret
