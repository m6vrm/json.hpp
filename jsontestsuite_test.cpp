#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>

#define JSON_IMPLEMENTATION
#define JSON_STRICT
#include "json.hpp"

#define COLOR_RESET "\033[0m"
#define COLOR_GOOD "\033[32m"  // green
#define COLOR_BAD "\033[31m"   // red
#define COLOR_OK "\033[33m"    // yellow

const char* TESTS[] = {
    "i_number_double_huge_neg_exp.json",
    "i_number_huge_exp.json",
    "i_number_neg_int_huge_exp.json",
    "i_number_pos_double_huge_exp.json",
    "i_number_real_neg_overflow.json",
    "i_number_real_pos_overflow.json",
    "i_number_real_underflow.json",
    "i_number_too_big_neg_int.json",
    "i_number_too_big_pos_int.json",
    "i_number_very_big_negative_int.json",
    "i_object_key_lone_2nd_surrogate.json",
    "i_string_1st_surrogate_but_2nd_missing.json",
    "i_string_1st_valid_surrogate_2nd_invalid.json",
    "i_string_UTF-16LE_with_BOM.json",
    "i_string_UTF-8_invalid_sequence.json",
    "i_string_UTF8_surrogate_U+D800.json",
    "i_string_incomplete_surrogate_and_escape_valid.json",
    "i_string_incomplete_surrogate_pair.json",
    "i_string_incomplete_surrogates_escape_valid.json",
    "i_string_invalid_lonely_surrogate.json",
    "i_string_invalid_surrogate.json",
    "i_string_invalid_utf-8.json",
    "i_string_inverted_surrogates_U+1D11E.json",
    "i_string_iso_latin_1.json",
    "i_string_lone_second_surrogate.json",
    "i_string_lone_utf8_continuation_byte.json",
    "i_string_not_in_unicode_range.json",
    "i_string_overlong_sequence_2_bytes.json",
    "i_string_overlong_sequence_6_bytes.json",
    "i_string_overlong_sequence_6_bytes_null.json",
    "i_string_truncated-utf-8.json",
    "i_string_utf16BE_no_BOM.json",
    "i_string_utf16LE_no_BOM.json",
    "i_structure_500_nested_arrays.json",
    "i_structure_UTF-8_BOM_empty_object.json",
    "n_array_1_true_without_comma.json",
    "n_array_a_invalid_utf8.json",
    "n_array_colon_instead_of_comma.json",
    "n_array_comma_after_close.json",
    "n_array_comma_and_number.json",
    "n_array_double_comma.json",
    "n_array_double_extra_comma.json",
    "n_array_extra_close.json",
    "n_array_incomplete.json",
    "n_array_incomplete_invalid_value.json",
    "n_array_inner_array_no_comma.json",
    "n_array_invalid_utf8.json",
    "n_array_items_separated_by_semicolon.json",
    "n_array_just_comma.json",
    "n_array_just_minus.json",
    "n_array_missing_value.json",
    "n_array_newlines_unclosed.json",
    "n_array_number_and_several_commas.json",
    "n_array_spaces_vertical_tab_formfeed.json",
    "n_array_star_inside.json",
    "n_array_unclosed.json",
    "n_array_unclosed_trailing_comma.json",
    "n_array_unclosed_with_new_lines.json",
    "n_array_unclosed_with_object_inside.json",
    "n_incomplete_false.json",
    "n_incomplete_null.json",
    "n_incomplete_true.json",
    "n_multidigit_number_then_00.json",
    "n_number_++.json",
    "n_number_+1.json",
    "n_number_+Inf.json",
    "n_number_-01.json",
    "n_number_-1.0..json",
    "n_number_-NaN.json",
    "n_number_.-1.json",
    "n_number_.2e-3.json",
    "n_number_0.1.2.json",
    "n_number_0.3e+.json",
    "n_number_0.3e.json",
    "n_number_0_capital_E+.json",
    "n_number_0_capital_E.json",
    "n_number_0e+.json",
    "n_number_0e.json",
    "n_number_1.0e+.json",
    "n_number_1.0e-.json",
    "n_number_1.0e.json",
    "n_number_1_000.json",
    "n_number_1eE2.json",
    "n_number_9.e+.json",
    "n_number_Inf.json",
    "n_number_NaN.json",
    "n_number_U+FF11_fullwidth_digit_one.json",
    "n_number_expression.json",
    "n_number_hex_1_digit.json",
    "n_number_hex_2_digits.json",
    "n_number_infinity.json",
    "n_number_invalid+-.json",
    "n_number_invalid-negative-real.json",
    "n_number_invalid-utf-8-in-bigger-int.json",
    "n_number_invalid-utf-8-in-exponent.json",
    "n_number_invalid-utf-8-in-int.json",
    "n_number_minus_infinity.json",
    "n_number_minus_sign_with_trailing_garbage.json",
    "n_number_minus_space_1.json",
    "n_number_neg_int_starting_with_zero.json",
    "n_number_neg_real_without_int_part.json",
    "n_number_neg_with_garbage_at_end.json",
    "n_number_real_garbage_after_e.json",
    "n_number_real_with_invalid_utf8_after_e.json",
    "n_number_starting_with_dot.json",
    "n_number_with_alpha.json",
    "n_number_with_alpha_char.json",
    "n_number_with_leading_zero.json",
    "n_object_bad_value.json",
    "n_object_bracket_key.json",
    "n_object_comma_instead_of_colon.json",
    "n_object_double_colon.json",
    "n_object_emoji.json",
    "n_object_garbage_at_end.json",
    "n_object_key_with_single_quotes.json",
    "n_object_missing_colon.json",
    "n_object_missing_key.json",
    "n_object_missing_semicolon.json",
    "n_object_missing_value.json",
    "n_object_no-colon.json",
    "n_object_non_string_key.json",
    "n_object_non_string_key_but_huge_number_instead.json",
    "n_object_repeated_null_null.json",
    "n_object_several_trailing_commas.json",
    "n_object_single_quote.json",
    "n_object_trailing_comment.json",
    "n_object_trailing_comment_open.json",
    "n_object_trailing_comment_slash_open_incomplete.json",
    "n_object_two_commas_in_a_row.json",
    "n_object_unquoted_key.json",
    "n_object_unterminated-value.json",
    "n_object_with_single_string.json",
    "n_object_with_trailing_garbage.json",
    "n_single_space.json",
    "n_string_1_surrogate_then_escape.json",
    "n_string_1_surrogate_then_escape_u.json",
    "n_string_1_surrogate_then_escape_u1.json",
    "n_string_1_surrogate_then_escape_u1x.json",
    "n_string_accentuated_char_no_quotes.json",
    "n_string_backslash_00.json",
    "n_string_escape_x.json",
    "n_string_escaped_backslash_bad.json",
    "n_string_escaped_ctrl_char_tab.json",
    "n_string_escaped_emoji.json",
    "n_string_incomplete_escape.json",
    "n_string_incomplete_escaped_character.json",
    "n_string_incomplete_surrogate.json",
    "n_string_incomplete_surrogate_escape_invalid.json",
    "n_string_invalid-utf-8-in-escape.json",
    "n_string_invalid_backslash_esc.json",
    "n_string_invalid_unicode_escape.json",
    "n_string_invalid_utf8_after_escape.json",
    "n_string_leading_uescaped_thinspace.json",
    "n_string_no_quotes_with_bad_escape.json",
    "n_string_single_doublequote.json",
    "n_string_single_quote.json",
    "n_string_single_string_no_double_quotes.json",
    "n_string_start_escape_unclosed.json",
    "n_string_unicode_CapitalU.json",
    "n_string_with_trailing_garbage.json",
    "n_structure_100000_opening_arrays.json",
    "n_structure_U+2060_word_joined.json",
    "n_structure_UTF8_BOM_no_data.json",
    "n_structure_angle_bracket_..json",
    "n_structure_angle_bracket_null.json",
    "n_structure_array_trailing_garbage.json",
    "n_structure_array_with_extra_array_close.json",
    "n_structure_array_with_unclosed_string.json",
    "n_structure_ascii-unicode-identifier.json",
    "n_structure_capitalized_True.json",
    "n_structure_close_unopened_array.json",
    "n_structure_comma_instead_of_closing_brace.json",
    "n_structure_double_array.json",
    "n_structure_end_array.json",
    "n_structure_incomplete_UTF8_BOM.json",
    "n_structure_lone-invalid-utf-8.json",
    "n_structure_lone-open-bracket.json",
    "n_structure_no_data.json",
    "n_structure_null-byte-outside-string.json",
    "n_structure_number_with_trailing_garbage.json",
    "n_structure_object_followed_by_closing_object.json",
    "n_structure_object_unclosed_no_value.json",
    "n_structure_object_with_comment.json",
    "n_structure_object_with_trailing_garbage.json",
    "n_structure_open_array_apostrophe.json",
    "n_structure_open_array_comma.json",
    "n_structure_open_array_object.json",
    "n_structure_open_array_open_object.json",
    "n_structure_open_array_open_string.json",
    "n_structure_open_array_string.json",
    "n_structure_open_object.json",
    "n_structure_open_object_close_array.json",
    "n_structure_open_object_comma.json",
    "n_structure_open_object_open_array.json",
    "n_structure_open_object_open_string.json",
    "n_structure_open_object_string_with_apostrophes.json",
    "n_structure_open_open.json",
    "n_structure_single_eacute.json",
    "n_structure_single_star.json",
    "n_structure_trailing_#.json",
    "n_structure_uescaped_LF_before_string.json",
    "n_structure_unclosed_array.json",
    "n_structure_unclosed_array_partial_null.json",
    "n_structure_unclosed_array_unfinished_false.json",
    "n_structure_unclosed_array_unfinished_true.json",
    "n_structure_unclosed_object.json",
    "n_structure_unicode-identifier.json",
    "n_structure_whitespace_U+2060_word_joiner.json",
    "n_structure_whitespace_formfeed.json",
    "y_array_arraysWithSpaces.json",
    "y_array_empty-string.json",
    "y_array_empty.json",
    "y_array_ending_with_newline.json",
    "y_array_false.json",
    "y_array_heterogeneous.json",
    "y_array_null.json",
    "y_array_with_1_and_newline.json",
    "y_array_with_leading_space.json",
    "y_array_with_several_null.json",
    "y_array_with_trailing_space.json",
    "y_number.json",
    "y_number_0e+1.json",
    "y_number_0e1.json",
    "y_number_after_space.json",
    "y_number_double_close_to_zero.json",
    "y_number_int_with_exp.json",
    "y_number_minus_zero.json",
    "y_number_negative_int.json",
    "y_number_negative_one.json",
    "y_number_negative_zero.json",
    "y_number_real_capital_e.json",
    "y_number_real_capital_e_neg_exp.json",
    "y_number_real_capital_e_pos_exp.json",
    "y_number_real_exponent.json",
    "y_number_real_fraction_exponent.json",
    "y_number_real_neg_exp.json",
    "y_number_real_pos_exponent.json",
    "y_number_simple_int.json",
    "y_number_simple_real.json",
    "y_object.json",
    "y_object_basic.json",
    "y_object_duplicated_key.json",
    "y_object_duplicated_key_and_value.json",
    "y_object_empty.json",
    "y_object_empty_key.json",
    "y_object_extreme_numbers.json",
    "y_object_long_strings.json",
    "y_object_simple.json",
    "y_object_with_newlines.json",
    "y_string_allowed_escapes.json",
    "y_string_backslash_and_u_escaped_zero.json",
    "y_string_backslash_doublequotes.json",
    "y_string_comments.json",
    "y_string_double_escape_a.json",
    "y_string_double_escape_n.json",
    "y_string_in_array.json",
    "y_string_in_array_with_leading_space.json",
    "y_string_nonCharacterInUTF-8_U+10FFFF.json",
    "y_string_nonCharacterInUTF-8_U+FFFF.json",
    "y_string_pi.json",
    "y_string_reservedCharacterInUTF-8_U+1BFFF.json",
    "y_string_simple_ascii.json",
    "y_string_space.json",
    "y_string_u+2028_line_sep.json",
    "y_string_u+2029_par_sep.json",
    "y_string_unescaped_char_delete.json",
    "y_string_unicode_2.json",
    "y_string_utf8.json",
    "y_string_with_del_character.json",
    "y_structure_lonely_false.json",
    "y_structure_lonely_int.json",
    "y_structure_lonely_negative_real.json",
    "y_structure_lonely_null.json",
    "y_structure_lonely_string.json",
    "y_structure_lonely_true.json",
    "y_structure_string_empty.json",
    "y_structure_trailing_newline.json",
    "y_structure_true_in_array.json",
    "y_structure_whitespace_array.json",
    "ny_object_escaped_null_in_key.json",                               // unicode
    "ny_object_string_unicode.json",                                    // unicode
    "ny_string_1_2_3_bytes_UTF-8_sequences.json",                       // unicode
    "ny_string_accepted_surrogate_pair.json",                           // unicode
    "ny_string_accepted_surrogate_pairs.json",                          // unicode
    "ny_string_escaped_control_character.json",                         // unicode
    "ny_string_escaped_noncharacter.json",                              // unicode
    "ny_string_last_surrogates_1_and_2.json",                           // unicode
    "ny_string_nbsp_uescaped.json",                                     // unicode
    "ny_string_null_escape.json",                                       // unicode
    "ny_string_one-byte-utf-8.json",                                    // unicode
    "ny_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.json",          // unicode
    "ny_string_three-byte-utf-8.json",                                  // unicode
    "ny_string_two-byte-utf-8.json",                                    // unicode
    "ny_string_uEscape.json",                                           // unicode
    "ny_string_uescaped_newline.json",                                  // unicode
    "ny_string_unicode.json",                                           // unicode
    "ny_string_unicodeEscapedBackslash.json",                           // unicode
    "ny_string_unicode_U+10FFFE_nonchar.json",                          // unicode
    "ny_string_unicode_U+1FFFE_nonchar.json",                           // unicode
    "ny_string_unicode_U+200B_ZERO_WIDTH_SPACE.json",                   // unicode
    "ny_string_unicode_U+2064_invisible_plus.json",                     // unicode
    "ny_string_unicode_U+FDD0_nonchar.json",                            // unicode
    "ny_string_unicode_U+FFFE_nonchar.json",                            // unicode
    "ny_string_unicode_escaped_double_quote.json",                      // unicode
    "yn_array_extra_comma.json",                                        // trailing comma
    "yn_array_number_and_comma.json",                                   // trailing comma
    "yn_number_-2..json",                                               // don't care
    "yn_number_0.e1.json",                                              // don't care
    "yn_number_2.e+3.json",                                             // don't care
    "yn_number_2.e-3.json",                                             // don't care
    "yn_number_2.e3.json",                                              // don't care
    "yn_number_real_without_fractional_part.json",                      // don't care
    "yn_object_lone_continuation_byte_in_key_and_trailing_comma.json",  // trailing comma
    "yn_object_trailing_comma.json",                                    // trailing comma
    "yn_object_trailing_comment_slash_open.json",                       // comment
    "yn_string_unescaped_ctrl_char.json",                               // don't care
    "yn_string_unescaped_newline.json",                                 // don't care
    "yn_string_unescaped_tab.json",                                     // don't care
};

static std::string read(const char* file);

int main() {
    int failures = 0;

    char path[128];
    for (std::size_t i = 0; i < sizeof(TESTS) / sizeof(*TESTS); ++i) {
        std::snprintf(path, sizeof(path), "JSONTestSuite/test_parsing/%s", TESTS[i]);
        std::string json_string = read(path);

        JSON json;
        JSON::Status status;
        bool success = json.parse(json_string, &status);

        const char* color = "";
        const char* reason = "";
        switch (TESTS[i][0]) {
            case 'y': {
                if (success) {
                    color = TESTS[i][1] != 'n' ? COLOR_GOOD : COLOR_OK;
                    reason = "PASS";
                } else {
                    color = COLOR_BAD;
                    reason = "SHOULD_PASS";
                    ++failures;
                }
            } break;
            case 'n': {
                if (!success) {
                    color = TESTS[i][1] != 'y' ? COLOR_GOOD : COLOR_OK;
                    reason = "FAIL";
                } else {
                    color = COLOR_BAD;
                    reason = "SHOULD_FAIL";
                    ++failures;
                }
            } break;
            case 'i': {
                color = COLOR_GOOD;
                if (success) {
                    reason = "PASS";
                } else {
                    reason = "FAIL";
                }
            } break;
            default:
                assert(false);
        }

        std::printf("%-70s %s%s%s", TESTS[i], color, reason, COLOR_RESET);
        if (!success)
            std::printf(" (%s)", JSON::status_string(status));
        std::printf("\n");
    }

    if (failures > 0) {
        std::printf("failed: %s%d%s\n", COLOR_BAD, failures, COLOR_RESET);
    } else {
        std::printf("success\n");
    }

    return failures;
}

static std::string read(const char* path) {
    FILE* file = std::fopen(path, "rb");
    if (file == nullptr) {
        std::perror(path);
        std::exit(1);
    }

    std::string string;
    char buf[1024];
    while (!std::feof(file) && !std::ferror(file)) {
        std::size_t n = std::fread(buf, 1, sizeof(buf), file);
        if (n == 0)
            break;
        string.append(buf, n);
    }

    std::fclose(file);
    return string;
}
