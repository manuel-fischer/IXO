#pragma once

#define IXO_PP_COMMA() ,
#define IXO_PP_NONE()

#define IXO_PP_CAT_(a, b) a##b
#define IXO_PP_CAT(a, b) IXO_PP_CAT_(a, b)

#define IXO_32TH(p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,...) p32
#define IXO_PARAM_SIZE(...) IXO_32TH(__VA_ARGS__, 32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1)

#define IXO_UNPACK(...) __VA_ARGS__
#define IXO_TUP_CAT(a, b) (IXO_UNPACK a, IXO_UNPACK b)

#define IXO_INVOKE_(macro, captup) macro captup
#define IXO_INVOKE(macro, capture, tuple) IXO_INVOKE_(macro, IXO_TUP_CAT(capture, tuple))

#define IXO_APPLY_EACH_1(macro, capture, tuple) IXO_INVOKE(macro, capture, tuple)
#define IXO_APPLY_EACH_2(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_1(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_3(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_2(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_4(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_3(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_5(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_4(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_6(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_5(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_7(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_6(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_8(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_7(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_9(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_8(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_10(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_9(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_11(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_10(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_12(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_11(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_13(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_12(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_14(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_13(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_15(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_14(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_16(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_15(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_17(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_16(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_18(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_17(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_19(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_18(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_20(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_19(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_21(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_20(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_22(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_21(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_23(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_22(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_24(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_23(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_25(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_24(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_26(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_25(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_27(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_26(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_28(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_27(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_29(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_28(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_30(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_29(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_31(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_30(macro, capture, __VA_ARGS__)
#define IXO_APPLY_EACH_32(macro, capture, tuple, ...) IXO_INVOKE(macro, capture, tuple) IXO_APPLY_EACH_31(macro, capture, __VA_ARGS__)

#define IXO_APPLY_EACH(macro, capture, ...) \
    IXO_PP_CAT(IXO_APPLY_EACH_, IXO_PARAM_SIZE(__VA_ARGS__))(macro, capture, __VA_ARGS__)
