
#include <xmmintrin.h>// <x86intrin.h>
#include <pmmintrin.h>
#include <emmintrin.h>

#include "mt_rand.h"


#define macro_unroll(func, cnt, args...)        \
    register unsigned block_cnt = (cnt) / 8;    \
    switch ((cnt) % 8) {            \
        case 0: do {                \
                func(args);         \
        case 7: func(args);         \
        case 6: func(args);         \
        case 5: func(args);         \
        case 4: func(args);         \
        case 3: func(args);         \
        case 2: func(args);         \
        case 1: func(args);         \
        } while (block_cnt--);      \
    }


#define natv_type(vect_bit_mag, oprnd_type) __m ## vect_bit_mag ## oprnd_type
#define natv_intrsc(vect_bit_mag, oper, oprnd_type, bit_mag) \
    _mm ## vect_bit_mag ## _ ##    oper        ##  _ ##  oprnd_type    ##      bit_mag
//         (128, 256, 512)   (set1, add, ..)    (epi, epu, si)      (8, 16, 32, 64, 128, ...)

#define natv_sse_type(oprnd_type) natv_type(128, oprnd_type)

#define natv_sse_intgl_t                      natv_sse_type(i)
#define natv_sse_intrsc(oper, oprnd_type, oprnd_bit_sz)        natv_intrsc(, oper, oprnd_type, oprnd_bit_sz)
#define natv_sse_intrsc_si(oper, args...)     natv_sse_intrsc(oper, si, 128)
#define natv_sse_intrsc_epi(oper, bit_mag)    natv_sse_intrsc(oper, epi, bit_mag)


#define lrgst_natv_intgl_t      natv_sse_intgl_t
#define lrgst_natv_intrs_si     natv_sse_intrsc_si
#define lrgst_natv_intrs_epi    natv_sse_intrsc_epi

// ************************************************************************************************
#ifdef __INTEL_COMPILER // icc doesn't support __nodebug__
#   define intrs_attrs  __attribute__((__always_inline__))
#else
#   define intrs_attrs  __attribute__((__always_inline__, __nodebug__))
#endif

#define intrsc_signat(ret_t) static __inline__ ret_t intrs_attrs


intrsc_signat(__m128i) _mm_extd_sign_bit_epi32(__m128i a) {
    return _mm_srai_epi32(a, 32);
}

intrsc_signat(__m128i) _mm_extd_sign_bit_epi64(__m128i a) {
    return _mm_srai_epi32(_mm_shuffle_epi32(a, _MM_SHUFFLE(3, 3, 1, 1)), 32);
}
// ************************************************************************************************


// **********************************************************************************************************

#define load_align_32       lrgst_natv_intrs_si(load)
#define load_align_64       load_align_32

#define load_32             lrgst_natv_intrs_si(lddqu)
#define load_64             load_32

#define store_align_32      lrgst_natv_intrs_si(store)
#define store_align_64      store_align_32

#define store_32            lrgst_natv_intrs_si(storeu)
#define store_64            store_32

#define broadcast_32        lrgst_natv_intrs_epi(set1, 32)
#define broadcast_64        lrgst_natv_intrs_epi(set1, 64x)

#define bitwise_and_32      lrgst_natv_intrs_si(and)
#define bitwise_and_64      bitwise_and_32

#define bitwise_xor_32      lrgst_natv_intrs_si(xor)
#define bitwise_xor_64      bitwise_xor_32

#define add_32              _mm_add_epi32
#define add_64              _mm_add_epi64

#define rshft_lgcl_imm_32   lrgst_natv_intrs_epi(srli, 32)
#define rshft_lgcl_imm_64   lrgst_natv_intrs_epi(srli, 64)

#define rshft_arth_imm_32   lrgst_natv_intrs_epi(srai, 32)
#define rshft_arth_imm_64   lrgst_natv_intrs_epi(srai, 64)

#define extd_sign_bit_32    lrgst_natv_intrs_epi(extd_sign_bit, 32)
#define extd_sign_bit_64    lrgst_natv_intrs_epi(extd_sign_bit, 64)

#define lshft_imm_32       lrgst_natv_intrs_epi(slli, 32)
#define lshft_imm_64       lrgst_natv_intrs_epi(slli, 64)

#define intgl_intrsc_f(name, bit_sz) name ## _ ## bit_sz

// ********************************************************************************************************

#define load_epi_f(bit_sz)              intgl_intrsc_f(load, bit_sz)
#define load_epi_align_f(bit_sz)        intgl_intrsc_f(load_align, bit_sz)

#define store_epi_f(bit_sz)             intgl_intrsc_f(store, bit_sz)
#define store_epi_align_f(bit_sz)       intgl_intrsc_f(store_align, bit_sz)

#define broadcast_epi_f(bit_sz)         intgl_intrsc_f(broadcast, bit_sz)
#define add_epi_f(bit_sz)               intgl_intrsc_f(add, bit_sz)
#define and_epi_f(bit_sz)               intgl_intrsc_f(bitwise_and, bit_sz)
#define xor_epi_f(bit_sz)               intgl_intrsc_f(bitwise_xor, bit_sz)
#define rshft_lgcl_imm_epi_f(bit_sz)    intgl_intrsc_f(rshft_lgcl_imm, bit_sz)
#define lshft_imm_f(bit_sz)             intgl_intrsc_f(lshft_imm, bit_sz)

#define extd_sign_bit_epi_f(bit_sz)    intgl_intrsc_f(extd_sign_bit, bit_sz)

// ************************************************************************************************

#define mt_oprtr(name, bit_mag) name ## _mt_ ## bit_mag

//#define oprn_t lrgst_natv_intgl_t

typedef lrgst_natv_intgl_t (*const intgl_loader)(const lrgst_natv_intgl_t *);
typedef void               (*const intgl_storer)(lrgst_natv_intgl_t *, lrgst_natv_intgl_t);
typedef lrgst_natv_intgl_t (*const intgl_32_brdcstr)(int);
typedef lrgst_natv_intgl_t (*const intgl_64_brdcstr)(long long int);
typedef lrgst_natv_intgl_t (*const bit_shftr)(lrgst_natv_intgl_t, int);

typedef lrgst_natv_intgl_t (*const intgl_bin_oper)(lrgst_natv_intgl_t, lrgst_natv_intgl_t);
typedef lrgst_natv_intgl_t (*const intgl_unr_oper)(lrgst_natv_intgl_t);


#define vect_intgl_oper_t(name, bit_mag) intgl_ ## bit_mag ## _ ## name

#define intgl_brdcstr_t(bit_mag)    vect_intgl_oper_t(brdcstr, bit_mag)

#define intgl_loadr_t(bit_mag)      intgl_loader
#define intgl_storr_t(bit_mag)      intgl_storer

#define intgl_bin_oprtr_t(bit_mag)  intgl_bin_oper
#define intgl_unr_oprtr_t(bit_mag)  intgl_unr_oper

#define intgl_shftr_imm_t(bit_mag)  bit_shftr

#define intgl_brdcstr(bit_size) intgl_ ## bit_size ## _brdcstr

#define load_mt(bit_mag)        &load_epi_f(bit_mag) // mt_oprtr(load,          bit_mag)
#define load_align_mt(bit_mag)  &load_epi_align_f(bit_mag) // mt_oprtr(load_align,    bit_mag)
#define store_mt(bit_mag)       &store_epi_f(bit_mag) // mt_oprtr(store,         bit_mag)
#define store_align_mt(bit_mag) &store_epi_align_f(bit_mag) // mt_oprtr(store_align,   bit_mag)

#define broadcast_mt(bit_mag)   &broadcast_epi_f(bit_mag) //mt_oprtr(broadcast,     bit_mag)
#define add_mt(bit_mag)         &add_epi_f(bit_mag) //mt_oprtr(add,           bit_mag)
#define and_mt(bit_mag)         &and_epi_f(bit_mag) // mt_oprtr(and,           bit_mag)
#define xor_mt(bit_mag)         &xor_epi_f(bit_mag) // mt_oprtr(xor,           bit_mag)
#define lshft_imm_mt(bit_mag)   &lshft_imm_f(bit_mag) // mt_oprtr(lshft_imm,     bit_mag)

#define rshft_lgcl_imm_mt(bit_mag)  &rshft_lgcl_imm_epi_f(bit_mag) // mt_oprtr(rshft_lgcl_imm, bit_mag)

#define extd_sign_bit_mt(bit_mag)   &extd_sign_bit_epi_f(bit_mag) //mt_oprtr(extd_sign_bit,     bit_mag)



#define mt_rand_vect_upd_state_f(bit_mag) mt_rand_vect_upd_state_ ## bit_mag


#define mt_def_intgl_oprtns(bit_mag)                                        \
    static const intgl_brdcstr_t(bit_mag) brdcst = broadcast_mt(bit_mag);   \
    static const intgl_loadr_t(bit_mag)                                     \
         load = load_mt(bit_mag)                                            \
        ,load_align = load_align_mt(bit_mag);                               \
    static const intgl_storr_t(bit_mag)                                     \
         store = store_mt(bit_mag)                                          \
        ,store_align = store_align_mt(bit_mag);                             \
    static const intgl_bin_oprtr_t(bit_mag)                                 \
         add = add_mt(bit_mag)                                              \
        ,and = and_mt(bit_mag)                                              \
        ,xor = xor_mt(bit_mag)                                              \
        ;                                                                   \
    static const intgl_shftr_imm_t(bit_mag)                                 \
        rshft = rshft_lgcl_imm_mt(bit_mag), lshft = lshft_imm_mt(bit_mag);  \
    static const intgl_unr_oprtr_t(bit_mag) extd_sign = extd_sign_bit_mt(bit_mag)


#define mt_coeff_32 0x9908B0DFU
#define mt_coeff_64 0xB5026F5AA96619E9LLU
#define mt_odd_mask(bit_mag) mt_coeff_ ## bit_mag

#define mt_seed_coef_32 1812433253U
#define mt_seed_coef_64 6364136223846793005LLU
#define mt_seed_coef(bit_mag) mt_seed_coef_ ## bit_mag

#define mt_mid_word_32 397
#define mt_mid_word_64 156
#define mt_mid_word(bit_mag) mt_mid_word_ ## bit_mag


#define mt_lower_mask_32 0x7FFFFFFFU
#define mt_lower_mask_64 0x7FFFFFFFLLU
#define mt_lower_mask(bit_mag) mt_lower_mask_ ## bit_mag

#define mt_upper_mask_32 0x80000000U
#define mt_upper_mask_64 0xFFFFFFFF80000000LLU
#define mt_upper_mask(bit_mag) mt_upper_mask_ ## bit_mag


mt_state_32_t mt_vect_32 = {{
    5489U, 1301868182U, 2938499221U, 2950281878U, 1875628136U, 751856242U, 944701696U, 2243192071U, 694061057U, 219885934U, 2066767472U, 3182869408U, 485472502U, 2336857883U, 1071588843U, 3418470598U, 951210697U, 3693558366U, 2923482051U, 1793174584U, 2982310801U, 1586906132U, 1951078751U, 1808158765U, 1733897588U, 431328322U, 4202539044U, 530658942U, 1714810322U, 3025256284U, 3342585396U, 1937033938U, 2640572511U, 1654299090U, 3692403553U, 4233871309U, 3497650794U, 862629010U, 2943236032U, 2426458545U, 1603307207U, 1133453895U, 3099196360U, 2208657629U, 2747653927U, 931059398U, 761573964U, 3157853227U, 785880413U, 730313442U, 124945756U, 2937117055U, 3295982469U, 1724353043U, 3021675344U, 3884886417U, 4010150098U, 4056961966U, 699635835U, 2681338818U, 1339167484U, 720757518U, 2800161476U, 2376097373U, 1532957371U, 3902664099U, 1238982754U, 3725394514U, 3449176889U, 3570962471U, 4287636090U, 4087307012U, 3603343627U, 202242161U, 2995682783U, 1620962684U, 3704723357U, 371613603U, 2814834333U, 2111005706U, 624778151U, 2094172212U, 4284947003U, 1211977835U, 991917094U, 1570449747U, 2962370480U, 1259410321U, 170182696U, 146300961U, 2836829791U, 619452428U, 2723670296U, 1881399711U, 1161269684U, 1675188680U, 4132175277U, 780088327U, 3409462821U, 1036518241U, 1834958505U, 3048448173U, 161811569U, 618488316U, 44795092U, 3918322701U, 1924681712U, 3239478144U, 383254043U, 4042306580U, 2146983041U, 3992780527U, 3518029708U, 3545545436U, 3901231469U, 1896136409U, 2028528556U, 2339662006U, 501326714U, 2060962201U, 2502746480U, 561575027U, 581893337U, 3393774360U, 1778912547U, 3626131687U, 2175155826U, 319853231U, 986875531U, 819755096U, 2915734330U, 2688355739U, 3482074849U, 2736559U, 2296975761U, 1029741190U, 2876812646U, 690154749U, 579200347U, 4027461746U, 1285330465U, 2701024045U, 4117700889U, 759495121U, 3332270341U, 2313004527U, 2277067795U, 4131855432U, 2722057515U, 1264804546U, 3848622725U, 2211267957U, 4100593547U, 959123777U, 2130745407U, 3194437393U, 486673947U, 1377371204U, 17472727U, 352317554U, 3955548058U, 159652094U, 1232063192U, 3835177280U, 49423123U, 3083993636U, 733092U, 2120519771U, 2573409834U, 1112952433U, 3239502554U, 761045320U, 1087580692U, 2540165110U, 641058802U, 1792435497U, 2261799288U, 1579184083U, 627146892U, 2165744623U, 2200142389U, 2167590760U, 2381418376U, 1793358889U, 3081659520U, 1663384067U, 2009658756U, 2689600308U, 739136266U, 2304581039U, 3529067263U, 591360555U, 525209271U, 3131882996U, 294230224U, 2076220115U, 3113580446U, 1245621585U, 1386885462U, 3203270426U, 123512128U, 12350217U, 354956375U, 4282398238U, 3356876605U, 3888857667U, 157639694U, 2616064085U, 1563068963U, 2762125883U, 4045394511U, 4180452559U, 3294769488U, 1684529556U, 1002945951U, 3181438866U, 22506664U, 691783457U, 2685221343U, 171579916U, 3878728600U, 2475806724U, 2030324028U, 3331164912U, 1708711359U, 1970023127U, 2859691344U, 2588476477U, 2748146879U, 136111222U, 2967685492U, 909517429U, 2835297809U, 3206906216U, 3186870716U, 341264097U, 2542035121U, 3353277068U, 548223577U, 3170936588U, 1678403446U, 297435620U, 2337555430U, 466603495U, 1132321815U, 1208589219U, 696392160U, 894244439U, 2562678859U, 470224582U, 3306867480U, 201364898U, 2075966438U, 1767227936U, 2929737987U, 3674877796U, 2654196643U, 3692734598U, 3528895099U, 2796780123U, 3048728353U, 842329300U, 191554730U, 2922459673U, 3489020079U, 3979110629U, 1022523848U, 2202932467U, 3583655201U, 3565113719U, 587085778U, 4176046313U, 3013713762U, 950944241U, 396426791U, 3784844662U, 3477431613U, 3594592395U, 2782043838U, 3392093507U, 3106564952U, 2829419931U, 1358665591U, 2206918825U, 3170783123U, 31522386U, 2988194168U, 1782249537U, 1105080928U, 843500134U, 1225290080U, 1521001832U, 3605886097U, 2802786495U, 2728923319U, 3996284304U, 903417639U, 1171249804U, 1020374987U, 2824535874U, 423621996U, 1988534473U, 2493544470U, 1008604435U, 1756003503U, 1488867287U, 1386808992U, 732088248U, 1780630732U, 2482101014U, 976561178U, 1543448953U, 2602866064U, 2021139923U, 1952599828U, 2360242564U, 2117959962U, 2753061860U, 2388623612U, 4138193781U, 2962920654U, 2284970429U, 766920861U, 3457264692U, 2879611383U, 815055854U, 2332929068U, 1254853997U, 3740375268U, 3799380844U, 4091048725U, 2006331129U, 1982546212U, 686850534U, 1907447564U, 2682801776U, 2780821066U, 998290361U, 1342433871U, 4195430425U, 607905174U, 3902331779U, 2454067926U, 1708133115U, 1170874362U, 2008609376U, 3260320415U, 2211196135U, 433538229U, 2728786374U, 2189520818U, 262554063U, 1182318347U, 3710237267U, 1221022450U, 715966018U, 2417068910U, 2591870721U, 2870691989U, 3418190842U, 4238214053U, 1540704231U, 1575580968U, 2095917976U, 4078310857U, 2313532447U, 2110690783U, 4056346629U, 4061784526U, 1123218514U, 551538993U, 597148360U, 4120175196U, 3581618160U, 3181170517U, 422862282U, 3227524138U, 1713114790U, 662317149U, 1230418732U, 928171837U, 1324564878U, 1928816105U, 1786535431U, 2878099422U, 3290185549U, 539474248U, 1657512683U, 552370646U, 1671741683U, 3655312128U, 1552739510U, 2605208763U, 1441755014U, 181878989U, 3124053868U, 1447103986U, 3183906156U, 1728556020U, 3502241336U, 3055466967U, 1013272474U, 818402132U, 1715099063U, 2900113506U, 397254517U, 4194863039U, 1009068739U, 232864647U, 2540223708U, 2608288560U, 2415367765U, 478404847U, 3455100648U, 3182600021U, 2115988978U, 434269567U, 4117179324U, 3461774077U, 887256537U, 3545801025U, 286388911U, 3451742129U, 1981164769U, 786667016U, 3310123729U, 3097811076U, 2224235657U, 2959658883U, 3370969234U, 2514770915U, 3345656436U, 2677010851U, 2206236470U, 271648054U, 2342188545U, 4292848611U, 3646533909U, 3754009956U, 3803931226U, 4160647125U, 1477814055U, 4043852216U, 1876372354U, 3133294443U, 3871104810U, 3177020907U, 2074304428U, 3479393793U, 759562891U, 164128153U, 1839069216U, 2114162633U, 3989947309U, 3611054956U, 1333547922U, 835429831U, 494987340U, 171987910U, 1252001001U, 370809172U, 3508925425U, 2535703112U, 1276855041U, 1922855120U, 835673414U, 3030664304U, 613287117U, 171219893U, 3423096126U, 3376881639U, 2287770315U, 1658692645U, 1262815245U, 3957234326U, 1168096164U, 2968737525U, 2655813712U, 2132313144U, 3976047964U, 326516571U, 353088456U, 3679188938U, 3205649712U, 2654036126U, 1249024881U, 880166166U, 691800469U, 2229503665U, 1673458056U, 4032208375U, 1851778863U, 2563757330U, 376742205U, 1794655231U, 340247333U, 1505873033U, 396524441U, 879666767U, 3335579166U, 3260764261U, 3335999539U, 506221798U, 4214658741U, 975887814U, 2080536343U, 3360539560U, 571586418U, 138896374U, 4234352651U, 2737620262U, 3928362291U, 1516365296U, 38056726U, 3599462320U, 3585007266U, 3850961033U, 471667319U, 1536883193U, 2310166751U, 1861637689U, 2530999841U, 4139843801U, 2710569485U, 827578615U, 2012334720U, 2907369459U, 3029312804U, 2820112398U, 1965028045U, 35518606U, 2478379033U, 643747771U, 1924139484U, 4123405127U, 3811735531U, 3429660832U, 3285177704U, 1948416081U, 1311525291U, 1183517742U, 1739192232U, 3979815115U, 2567840007U, 4116821529U, 213304419U, 4125718577U, 1473064925U, 2442436592U, 1893310111U, 4195361916U, 3747569474U, 828465101U, 2991227658U, 750582866U, 1205170309U, 1409813056U, 678418130U, 1171531016U, 3821236156U, 354504587U, 4202874632U, 3882511497U, 1893248677U, 1903078632U, 26340130U, 2069166240U, 3657122492U, 3725758099U, 831344905U, 811453383U, 3447711422U, 2434543565U, 4166886888U, 3358210805U, 4142984013U, 2988152326U, 3527824853U, 982082992U, 2809155763U, 190157081U, 3340214818U, 2365432395U, 2548636180U, 2894533366U, 3474657421U, 2372634704U, 2845748389U, 43024175U, 2774226648U, 1987702864U, 3186502468U, 453610222U, 4204736567U, 1392892630U, 2471323686U, 2470534280U, 3541393095U, 4269885866U, 3909911300U, 759132955U, 1482612480U, 667715263U, 1795580598U, 2337923983U, 3390586366U, 581426223U, 1515718634U, 476374295U, 705213300U, 363062054U, 2084697697U, 2407503428U, 2292957699U, 2426213835U, 2199989172U, 1987356470U, 4026755612U, 2147252133U, 270400031U, 1367820199U, 2369854699U, 2844269403U, 79981964U,
    5489U, 1301868182U, 2938499221U, 2950281878U //, 1875628136U, 751856242U, 944701696U, 2243192071U, 694061057U, 219885934U, 2066767472U, 3182869408U, 485472502U, 2336857883U, 1071588843U, 3418470598U
}, mt_states_cnt(&mt_vect_32)};

mt_state_64_t mt_vect_64 = {{
    5489LLU, 13057201162865595358LLU, 10476979627314799022LLU, 15076282145854160703LLU, 4028258760921719184LLU, 16400131027729929813LLU, 681049467949274916LLU, 1166424544479915355LLU, 12669671669325274631LLU, 3923681680445358570LLU, 10843524099671305260LLU, 9320087349666649633LLU, 18036750184230437171LLU, 15162073532206564733LLU, 6406996757156837684LLU, 8927855092125653344LLU, 7287101680298317085LLU, 14285962336228661757LLU, 16767098162355983288LLU, 3083970833968823538LLU, 16292429955202811038LLU, 2462140788281684654LLU, 14987206012938009260LLU, 1755961132248244698LLU, 11853308388629125482LLU, 15567715879394119521LLU, 12922380697022943828LLU, 10568493380422968121LLU, 6468114481096881787LLU, 6912714088192792975LLU, 11676810063224680468LLU, 7989628851951361533LLU, 9980521080467753324LLU, 11628798235400288887LLU, 3042835494701912499LLU, 10149139922063010202LLU, 258211445411067868LLU, 12292608484108957137LLU, 5167437948048335677LLU, 11526653342107776435LLU, 9186605994989076293LLU, 4106436007230823197LLU, 1482400223179564867LLU, 18329651462931014642LLU, 12828698185960104073LLU, 9435381729478913436LLU, 10988179007923054324LLU, 16279301207772373869LLU, 213769070704315526LLU, 2960748844084063679LLU, 10067976150718286789LLU, 9138367034755369774LLU, 13806268603918059639LLU, 1680185388186896326LLU, 10291061633078204420LLU, 14465151537550734149LLU, 15488623881140223366LLU, 3741484074564668314LLU, 1918677755306815564LLU, 7317293401479426455LLU, 4481774452245242266LLU, 13177439052661313103LLU, 14190197572724422343LLU, 11391962132027874483LLU, 14461854875984255581LLU, 78864998355633351LLU, 13375647221931413565LLU, 13091373515385904214LLU, 6049165922138400520LLU, 11416142809731847130LLU, 18197073924412990782LLU, 829354708239552256LLU, 7594476051345711944LLU, 10772269459197422366LLU, 9316238444709656630LLU, 820820292010192239LLU, 10370107716384591311LLU, 8321593491012460630LLU, 9226632414975219865LLU, 1121855342335555726LLU, 2523212579397444422LLU, 15150453816516406687LLU, 4357348246254069950LLU, 1475811360713763769LLU, 14612290868631353049LLU, 13002835200640305831LLU, 8841644283678816855LLU, 9422668006636366709LLU, 9762262470164229443LLU, 15759907042128835526LLU, 5472764997485778171LLU, 7662827925729932877LLU, 17501417670658457528LLU, 3930325588003666236LLU, 5474808447603626986LLU, 6720003803709822382LLU, 1855653125818638627LLU, 17923060195536629896LLU, 1006421872699162065LLU, 8593337867538992416LLU, 4799988366622961201LLU, 13724225912419217109LLU, 11153468036382729521LLU, 3227521569234212702LLU, 14253271084513918446LLU, 12720552637491820050LLU, 3131078640163418426LLU, 13204035025191316893LLU, 1242013424098797151LLU, 10309732291143311392LLU, 14812467554029601896LLU, 6588807180369779774LLU, 15873501535677970563LLU, 11172284785033359089LLU, 12301618027847470633LLU, 11068608208873034498LLU, 11428326129399486324LLU, 5094087545013561907LLU, 500288200114796864LLU, 2634392864069587127LLU, 10024792545775434147LLU, 10356221529759776966LLU, 11136380342514802414LLU, 340782545860183031LLU, 7508198866667469799LLU, 7289875136835936747LLU, 17690097813874199712LLU, 2626238110689777190LLU, 16717695660713672494LLU, 3595834123325255274LLU, 6135238878624366372LLU, 9938504311934907652LLU, 11347072173565906066LLU, 9372835856550536661LLU, 2901728271276724305LLU, 9858149244111900100LLU, 16544617798517122646LLU, 11622889926249457786LLU, 9979924837559772578LLU, 14196945190861012395LLU, 2223272152803307284LLU, 5190516807419032337LLU, 3235768569839659614LLU, 7682633656132343061LLU, 13733309948923027732LLU, 16911272487285603183LLU, 16702635121049437838LLU, 6161415984776321628LLU, 12717629078983493101LLU, 2358424909955325080LLU, 12297813174132617038LLU, 9911119942162973939LLU, 14656296979938373109LLU, 5179190586448371415LLU, 11090893096306857528LLU, 11656051587341971149LLU, 2619718836853156863LLU, 167424595420134768LLU, 1643007456521706830LLU, 4530990928200931669LLU, 4691242637059006353LLU, 3245172607167855857LLU, 3826074447196161535LLU, 3017613396914933622LLU, 17340905364626031202LLU, 7485046344904985266LLU, 4965505580881047325LLU, 7607870693563722899LLU, 7474217805999604818LLU, 9839820025668071488LLU, 3904404505428916804LLU, 9096143925090925215LLU, 11720022622728597618LLU, 14607455239072224349LLU, 9652489256075507508LLU, 16157915074085584685LLU, 8844691517984910790LLU, 4655454640787506604LLU, 13027405036051698459LLU, 7614616053181367064LLU, 7581798355918172953LLU, 15422484141350085613LLU, 7273144328931681164LLU, 4809879802957181824LLU, 8173340538785729893LLU, 8978995124845705037LLU, 1098023286586191126LLU, 3673056527006128025LLU, 10771848665549917601LLU, 2556126669642826596LLU, 5853974322212222290LLU, 4132488280061906262LLU, 7632389934273528542LLU, 9864709072803865332LLU, 1026796482661462016LLU, 1419617114693595331LLU, 3962155586201817099LLU, 667987996344895412LLU, 8873514502505981802LLU, 651162605589119894LLU, 17797581581324995622LLU, 15976116878184660554LLU, 612180284401625759LLU, 5667627227252711358LLU, 10804568037840393823LLU, 13480141817918853670LLU, 1066512862997122338LLU, 3604813770717933001LLU, 13585907467660805157LLU, 205740876326491308LLU, 5991394416108877582LLU, 14926153760506158966LLU, 1763245647862174565LLU, 7472896455769818262LLU, 1880205322011031649LLU, 4964758817614792932LLU, 9867509509583481881LLU, 10312058868395878040LLU, 17252972030239322092LLU, 2606539039210012382LLU, 12769631308639825890LLU, 13775140203463199549LLU, 11099918903372708849LLU, 13347825623771273110LLU, 10911113188423225828LLU, 3460604650247618639LLU, 18367317190899220421LLU, 8882227645936398513LLU, 17724301884678217684LLU, 5689627886741111472LLU, 9758983823681554691LLU, 4544309240290776340LLU, 11463612010490044780LLU, 1863376090611217215LLU, 15532585436324660221LLU, 2614370430655215249LLU, 8917872921271699305LLU, 6432650944098428469LLU, 2156285533792683026LLU, 16620843026246231577LLU, 7840248012245686658LLU, 8817762536320809464LLU, 11411624210052135095LLU, 14469479953922933700LLU, 7687504684721677295LLU, 3569379597009150923LLU, 16298388750432321701LLU, 3546604078275180581LLU, 14090163417090112121LLU, 1483598196549275243LLU, 842296961800625865LLU, 3395823622991339856LLU, 2860049050133253132LLU, 15982791582006104857LLU, 2089985782673048208LLU, 16970930680417346639LLU, 5169822013739423324LLU, 10286951961495655002LLU, 2382826956176138874LLU, 16598361065592133237LLU, 17932005381186616770LLU, 16288375750215523058LLU, 10795004077740592227LLU, 8767132529733815572LLU, 56240711443019961LLU, 16559497623279599758LLU, 110008580074802387LLU, 11565107589793869602LLU, 8340806487881443756LLU, 15870344620874033014LLU, 11296081153908292511LLU, 7302467602367798952LLU, 67243528223816645LLU, 3486356707203513778LLU, 12062986918467299164LLU, 819578200798056089LLU, 18327439140423416057LLU, 14368763774382050055LLU, 15153510095141989578LLU, 3341425261026301804LLU, 773058210352526100LLU, 8392504547028739997LLU, 16740785353247611782LLU, 3373348860032225916LLU, 2701382140093875432LLU, 6671463639189304805LLU, 2761278783662691890LLU, 4685122996515124713LLU, 2654082339795866344LLU, 11329882967399066601LLU, 15952333297690283633LLU, 1697237544920553773LLU, 12370315011795239181LLU, 12798146676828103112LLU, 6070340910131537832LLU, 1447608530827808988LLU, 10598487560452381652LLU, 9074424128904564679LLU, 10368088978608816376LLU, 14242160977535644445LLU, 10536783946433683314LLU, 9271707826703226845LLU, 16651953013385761889LLU, 17192290660721538153LLU, 3817850688440651218LLU, 12138791431534730523LLU, 15752446791766328727LLU, 13797089951075641399LLU, 3884892512265821573LLU, 13501119693269626006LLU, 6429997517378945850LLU, 14292992949928449942LLU,
    5489LLU, 13057201162865595358LLU //, 10476979627314799022LLU, 15076282145854160703LLU, 4028258760921719184LLU, 16400131027729929813LLU, 681049467949274916LLU, 1166424544479915355LLU
}, mt_states_cnt(&mt_vect_64)};
// ^^^^^^^^^ Initial 32 and 64 bit mersenne twister random states ... (the state assuming being seeded with 5489)

// pre-calculate mod indices ....
static const unsigned short mod_indices_32[] = {397, 401, 405, 409, 413, 417, 421, 425, 429, 433, 437, 441, 445, 449, 453, 457, 461, 465, 469, 473, 477, 481, 485, 489, 493, 497, 501, 505, 509, 513, 517, 521, 525, 529, 533, 537, 541, 545, 549, 553, 557, 561, 565, 569, 573, 577, 581, 585, 589, 593, 597, 601, 605, 609, 613, 617, 621, 1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 129, 133, 137, 141, 145, 149, 153, 157, 161, 165, 169, 173, 177, 181, 185, 189, 193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 237, 241, 245, 249, 253, 257, 261, 265, 269, 273, 277, 281, 285, 289, 293, 297, 301, 305, 309, 313, 317, 321, 325, 329, 333, 337, 341, 345, 349, 353, 357, 361, 365, 369, 373, 377, 381, 385, 389, 393};
// ^^^^^^^ we can do first 56 tbl_entries in parallel, since they don't access previous tbl_entries, then we do the rest ...
static const unsigned short mod_indices_64[] = {156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 188, 190, 192, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 254, 256, 258, 260, 262, 264, 266, 268, 270, 272, 274, 276, 278, 280, 282, 284, 286, 288, 290, 292, 294, 296, 298, 300, 302, 304, 306, 308, 310, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148, 150, 152, 154};
// ^^^^^^^ we can do first 78 tbl_entries in parallel,
#define mod_indices(bit_mag) mod_indices_ ## bit_mag

#define mt_state_vector_cnt(mt, v) (mt_state_word_cnt(mt_state_t(mt))  / (sizeof(v)/sizeof(mt_state_t(mt))))

#define mt_rand_upd_entry(bit_mag, index)  ({                                                       \
    temp = add(and(upper_mask, load_align(&entries[index])), and(lower_mask, load(&next[index])));  \
    temp = xor(xor(rshft(temp, 1), load((const oprn_t *)&states[mods[index]])), and(odd_mask, extd_sign(lshft(temp, bit_mag - 1))));\
    store_align((oprn_t *)&entries[index], temp);                                                   \
})

typedef unsigned char (*cache_line_t)[64]; // <<<<<< x86 cache line is 64 bytes ...
// x86 cache line is 64 bytes ...
// __builtin_prefetch, loads a cache line takes 2 optional args,
//      first (rw) value 0(default) read, 1 for write,
//      second (locality) value 0 (no locality evit from cache after access), 1 (low), 2(moderate), 3(high) leave it in the cache as much as possible.
#define load_into_cpu_cache(mem, byte_cnt, read_write, locality) ({ \
    register cache_line_t line = (void *)mem;                       \
    macro_unroll(__builtin_prefetch, (byte_cnt)/sizeof(*line), line++, read_write, locality);\
})

#define mt_rand_vect_upd_state_tmpl(mt, bit_mag)                                \
    void mt_rand_vect_upd_state_f(bit_mag) () {                                 \
                                                                                \
        typedef lrgst_natv_intgl_t              oprn_t;                         \
        typedef mt_state_t(&mt)                 state_t;                        \
        typedef typeof(mod_indices(bit_mag)[0]) mod_indx_t;                     \
                                                                                \
        mt_def_intgl_oprtns(bit_mag);                                           \
                                                                                \
        static const unsigned char                                              \
            vect_states_cnt = mt_state_vector_cnt(&mt, oprn_t);                 \
        static const state_t *const states = (void *)&mt;                       \
        static const oprn_t                                                     \
             *const entries = (void *)&mt                                       \
            ,*const next    = (void *)((state_t *)&mt + 1)                      \
            ;                                                                   \
        static const mod_indx_t *const mods = mod_indices(bit_mag);             \
        register oprn_t                                                         \
             upper_mask  = brdcst(mt_upper_mask(bit_mag))                       \
            ,lower_mask  = brdcst(mt_lower_mask(bit_mag))                       \
            ,odd_mask    = brdcst(mt_odd_mask(bit_mag))                         \
            ,temp, temp1                                                        \
            ;                                                                   \
        load_into_cpu_cache(mods, sizeof(mod_indices(bit_mag)), 0, 0);          \
        mt_rand_upd_entry(bit_mag, 0);                                          \
        store_align((oprn_t *)&entries[vect_states_cnt], temp);                 \
        register unsigned char index;                                           \
        for (index = 1; index < vect_states_cnt; index++)                       \
            mt_rand_upd_entry(bit_mag, index);                                  \
    }


mt_rand_vect_upd_state_tmpl(mt_vect_32, 32)

mt_rand_vect_upd_state_tmpl(mt_vect_64, 64)




#define comp_likely(bool_expr)      __builtin_expect(bool_expr, 1)
#define comp_unlikely(bool_expr)    __builtin_expect(bool_expr, 0)


#define mt_rand_vect_state_updtr(bit_mag)


#define mt_rand_vect_masks_32() \
    oprn_t mask_0 = brdcst(2636928640U), mask_1 = brdcst(4022730752U)

#define mt_rand_vect_masks_64()                     \
    oprn_t                                          \
         mask_0 = brdcst(0x5555555555555555ULL)     \
        ,mask_1 = brdcst(0x71D67FFFEDA60000ULL)     \
        ,mask_2 = brdcst(0xFFF7EEE000000000ULL)


#define mt_rand_vect_def_masks(bit_mag) mt_rand_vect_masks_ ## bit_mag()

#define mt_vect_tamper_bits_32(x)               \
    x = xor(rshft(x, 11), x);                   \
    x = xor(and(mask_0, lshft(x, 7)), x);       \
    x = xor(and(mask_1, lshft(x, 15)), x);      \
    x = xor(rshft(x, 18), x)

#define mt_vect_tamper_bits_64(x)               \
    x = xor(and(mask_0, rshft(x, 29)), x);      \
    x = xor(and(mask_1, lshft(x, 17)), x);      \
    x = xor(and(mask_2, lshft(x, 37)), x);      \
    x = xor(rshft(x, 43), x)

#define mt_vect_bit_tamprr(bit_mag) mt_vect_tamper_bits_ ## bit_mag


// work with 64 bytes at a time (single cache line ...)
#define populate_cache_32(mt, bit_mag)                                      \
    register unsigned indx; register oprn_t temp0, temp1, temp2, temp3;\
    for (indx = 0; indx < mt_state_vector_cnt(mt, oprn_t); indx += 4) {     \
        temp0 = load_align(&src[indx]);              \
        temp1 = load_align(&src[indx + 1]);          \
        temp2 = load_align(&src[indx + 2]);          \
        temp3 = load_align(&src[indx + 3]);          \
        mt_vect_bit_tamprr(bit_mag)(temp0);                                 \
        mt_vect_bit_tamprr(bit_mag)(temp1);                                 \
        mt_vect_bit_tamprr(bit_mag)(temp2);                                 \
        mt_vect_bit_tamprr(bit_mag)(temp3);                                 \
        store_align((oprn_t *)&dest[indx],     temp0);          \
        store_align((oprn_t *)&dest[indx + 1], temp1);          \
        store_align((oprn_t *)&dest[indx + 2], temp2);          \
        store_align((oprn_t *)&dest[indx + 3], temp3);          \
    }


#define populate_cache_64(mt, bit_mag)  \
    register unsigned long long indx; register oprn_t temp0, temp1, temp2, temp3;                             \
    for (indx = 0; indx < mt_state_vector_cnt(mt, oprn_t); indx += 4) {  \
        temp0 = load_align(&src[indx]);              \
        temp1 = load_align(&src[indx + 1]);          \
        temp2 = load_align(&src[indx + 2]);          \
        temp3 = load_align(&src[indx + 3]);          \
        mt_vect_bit_tamprr(bit_mag)(temp0);          \
        mt_vect_bit_tamprr(bit_mag)(temp1);          \
        mt_vect_bit_tamprr(bit_mag)(temp2);          \
        mt_vect_bit_tamprr(bit_mag)(temp3);          \
        store_align((oprn_t *)&dest[indx],     temp0);          \
        store_align((oprn_t *)&dest[indx + 1], temp1);          \
        store_align((oprn_t *)&dest[indx + 2], temp2);          \
        store_align((oprn_t *)&dest[indx + 3], temp3);          \
    }

//register oprn_t temp0;              \
//    index = 0;                          \
//    macro_unroll(tempered_bits_64, mt_state_vector_cnt(mt, oprn_t), mt)

#define populate_cache(bit_mag) populate_cache_ ## bit_mag


#define mt_rand_vect_tmpl(mt, bit_mag)                                          \
    mt_state_t(mt) mt_rand_vect_ ## bit_mag () {                                \
        typedef lrgst_natv_intgl_t oprn_t;                                      \
        typedef mt_state_t(mt) state_t;                                         \
                                                                                \
        static state_t __attribute__ ((aligned (sizeof(oprn_t))))               \
            tempered_bits [mt_states_cnt(mt)];                                  \
                                                                                \
        if (comp_likely(mt_index(mt) < mt_states_cnt(mt)))                      \
            return tempered_bits[mt_inc_index(mt)];                             \
                                                                                \
        mt_rand_vect_upd_state_f(bit_mag)();                                    \
        static const oprn_t *const dest = (void *)tempered_bits, *const src = (void *)mt;\
        mt_def_intgl_oprtns(bit_mag);                                           \
        mt_rand_vect_def_masks(bit_mag);                                        \
        populate_cache(bit_mag)(mt, bit_mag);                                   \
        mt_set_index(mt, 1);                                                    \
        return tempered_bits[0];                                                \
}

mt_rand_vect_tmpl(&mt_vect_32, 32);

mt_rand_vect_tmpl(&mt_vect_64, 64);