#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* Normal world API */

/*!
 * \brief dfa_init
 * Initialize the DFA subsystem.
 * \param main_start Start address of main function
 * \param main_end End address of main function
 * \param u_start Start address of user code to be attested
 * \param u_end End address of user code to be attested
 */
uint32_t dfa_init(const cfa_addr_t main_start, const cfa_addr_t main_end,
		  const btbl_entry_t *btbl_start, const btbl_entry_t *btbl_end,
		  const ltbl_entry_t *ltbl_start, const ltbl_entry_t *ltbl_end);


/*!
 * \brief dfa_quote
 * Quote the current digest value.
 * \param out Output buffer for storing the digest quote
 * \param outlen Length of the output buffer for storing the digest quote
 */
uint32_t dfa_quote(const uint8_t *user_data, const uint32_t user_data_len,
		   uint8_t *out, uint32_t *out_len);
