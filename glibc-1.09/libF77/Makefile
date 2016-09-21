
subdir	:= libF77

headers	:= f2c.h

defines := $(defines) -D_POSIX_SOURCE -DNON_UNIX_STDIO

routines := FVersion d_cosh derfc_ hl_le pow_dd r_exp s_paus abort_ d_dim \
	    ef1asc_ hl_lt pow_di r_imag s_rnge c_abs d_exp ef1cmc_ i_abs \
	    pow_hh r_int s_stop c_cos d_imag erf_ i_dim pow_ii r_lg10 sig_die \
	    c_div d_int erfc_ i_dnnt r_log signal_ c_exp d_lg10 \
	    getarg_ i_indx pow_ri r_mod system_ c_log d_log getenv_ i_len \
	    pow_zi r_nint z_abs c_sin d_mod h_abs i_mod pow_zz r_sign z_cos \
	    c_sqrt d_nint h_dim i_nint r_abs r_sin z_div fcabs d_prod h_dnnt \
	    i_sign r_acos r_sinh z_exp d_abs d_sign h_indx iargc_ r_asin \
	    r_sqrt z_log d_acos d_sin h_len l_ge r_atan r_tan z_sin d_asin \
	    d_sinh h_mod l_gt r_atn2 r_tanh z_sqrt d_atan d_sqrt h_nint l_le \
	    r_cnjg s_cat d_atn2 d_tan h_sign l_lt r_cos s_catow d_cnjg d_tanh \
	    hl_ge fmain r_cosh s_cmp d_cos derf_ hl_gt pow_ci r_dim s_copy

extra-objs := libF77.a

install-lib := libF77.a

# F2C's code is not too clean.
override +gccwarn := -w

include ../Rules

$(objpfx)libF77.a: $(dep-dummy-lib); $(make-dummy-lib)
lib: $(objpfx)libF77.a

