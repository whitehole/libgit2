#include "blame_helpers.h"

void hunk_message(size_t idx, const git_blame_hunk *hunk, const char *fmt, ...)
{
	va_list arglist;

	printf("Hunk %zd (line %d +%d): ", idx,
			hunk->final_start_line_number, hunk->lines_in_hunk-1);

	va_start(arglist, fmt);
	vprintf(fmt, arglist);
	va_end(arglist);

	printf("\n");
}

void check_blame_hunk_index(git_repository *repo, git_blame *blame, int idx,
		int start_line, int len, const char *commit_id, const char *orig_path)
{
	git_object *obj;
	char expected[41] = {0}, actual[41] = {0};
	const git_blame_hunk *hunk = git_blame_get_hunk_byindex(blame, idx);
	cl_assert(hunk);

	if (!strncmp(commit_id, "0000", 4)) {
		strcpy(expected, "0000000000000000000000000000000000000000");
	} else {
		cl_git_pass(git_revparse_single(&obj, repo, commit_id));
		git_oid_fmt(expected, git_object_id(obj));
	}

	if (hunk->final_start_line_number != start_line) {
		hunk_message(idx, hunk, "mismatched start line number: expected %d, got %d",
				start_line, hunk->final_start_line_number);
	}
	cl_assert_equal_i(hunk->final_start_line_number, start_line);

	if (hunk->lines_in_hunk != len) {
		hunk_message(idx, hunk, "mismatched line count: expected %d, got %d",
				len, hunk->lines_in_hunk);
	}
	cl_assert_equal_i(hunk->lines_in_hunk, len);

	git_oid_fmt(actual, &hunk->final_commit_id);
	if (strcmp(expected, actual)) {
		hunk_message(idx, hunk, "has mismatched original id (got %s, expected %s)\n",
				actual, expected);
	}
	cl_assert_equal_s(actual, expected);
	if (strcmp(hunk->orig_path, orig_path)) {
		hunk_message(idx, hunk, "has mismatched original path (got '%s', expected '%s')\n",
				hunk->orig_path, orig_path);
	}
	cl_assert_equal_s(hunk->orig_path, orig_path);

	git_object_free(obj);
}

