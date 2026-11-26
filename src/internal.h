#ifndef LEDGER_INTERNAL_H
#define LEDGER_INTERNAL_H

#include <stdbool.h>

/* 라이브러리 내부에서만 쓰는 함수. 공개 API는 include/ledger/ 아래에 둔다. */

/* 직전 SQL의 SQLSTATE가 code(5글자)와 같은지. 예: "23505" = 유일성 제약 위반 */
bool ledger_db_sqlstate_is(const char *code);

/* 직전 SQL 오류를 stderr에 남긴다. where에는 함수명과 단계를 적는다. */
void ledger_db_log_error(const char *where);

#endif
