#include "dfa_stub.h"

/* Normal world API */

uint32_t dfa_init(const cfa_addr_t main_start, const cfa_addr_t main_end,
		  const btbl_entry_t *btbl_start, const btbl_entry_t *btbl_end,
		  const ltbl_entry_t *ltbl_start, const ltbl_entry_t *ltbl_end)
{
	cfa_init_params_t init_params;
	init_params.main_start = main_start;
	init_params.main_end = main_end;
	init_params.btbl_start = btbl_start;
	init_params.btbl_end = btbl_end;
	init_params.ltbl_start = ltbl_start;
	init_params.ltbl_end = ltbl_end;

    // pipe
	return smcall(CFA_EVENT_INIT, (uint32_t)&init_params, 0, 0);
}



uint32_t dfa_quote(const uint8_t *user_data, const uint32_t user_data_len,
		   uint8_t *out, uint32_t *out_len)
{
	cfa_quote_params_t quote_params;
	quote_params.user_data = user_data;
	quote_params.user_data_len = user_data_len;
	quote_params.out = out;
	quote_params.out_len = out_len;

    // pipe
	return smcall(CFA_EVENT_QUOTE, (uint32_t)&quote_params, 0, 0);
}

