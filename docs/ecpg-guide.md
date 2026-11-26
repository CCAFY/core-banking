# C + ECPG 안내 (Java/Spring 경험자용)

ECPG는 C 소스 안에 SQL을 직접 쓰게 해 주는 PostgreSQL의 전처리기다.
`.pgc` 파일을 `ecpg`가 일반 `.c` 파일로 바꾸고, 그걸 컴파일한다. 빌드가 알아서 해 준다.
Oracle의 Pro\*C와 같은 계열(임베디드 SQL)이라 코드 모양이 비슷하다.

## Java와 무엇이 다른가

| Java / Spring | C + ECPG |
|---|---|
| `@Transactional` | 없다. 함수 끝에서 직접 `EXEC SQL COMMIT`, 오류 경로에서 직접 `EXEC SQL ROLLBACK` |
| `PreparedStatement`의 `?` | 호스트 변수 `:h_name` |
| `SQLException`이 던져짐 | 예외가 없다. SQL 한 문장마다 `sqlca.sqlcode`를 직접 확인한다 |
| 조회 결과 없음 (`Optional.empty()`) | `sqlca.sqlcode == ECPG_NOT_FOUND` (값 100) |
| `try` / `finally` | `goto rollback`으로 출구를 한 곳에 모은다 |
| `String` | 크기가 정해진 `char` 배열. 복사는 `snprintf`만 쓴다 |
| `long` | `long long` (호스트 변수), `int64_t` (함수 인자) |
| 가비지 컬렉터 | 없다. 동적 할당(`malloc`)은 되도록 쓰지 않는다 |
| 커넥션 풀 | 프로세스당 접속 하나 |
| JUnit | GoogleTest (테스트 코드만 C++) |

## 기본 모양

`src/db.pgc`의 `ledger_db_ping`이 가장 작은 예시다. 구조는 항상 같다.

```c
EXEC SQL INCLUDE sqlca;              /* 파일마다 한 번 */

ledger_rc some_function(const char *name, long long *out)
{
    EXEC SQL BEGIN DECLARE SECTION;  /* SQL에서 쓸 변수는 여기에 선언 */
    char h_name[41];                 /* VARCHAR(40) + 끝의 '\0' 한 칸 */
    long long h_value;
    EXEC SQL END DECLARE SECTION;
    ledger_rc rc = LEDGER_ERR_DB;

    snprintf(h_name, sizeof(h_name), "%s", name);

    EXEC SQL SELECT value INTO :h_value FROM some_table WHERE name = :h_name;
    if (sqlca.sqlcode == ECPG_NOT_FOUND) {   /* 한 건도 없음 */
        /* rc를 알맞은 결과 코드로 정한다 */
        goto rollback;
    }
    if (sqlca.sqlcode < 0) {                 /* 오류 */
        ledger_db_log_error("some_function: select");
        goto rollback;
    }

    EXEC SQL COMMIT;
    if (sqlca.sqlcode < 0) {
        ledger_db_log_error("some_function: commit");
        goto rollback;
    }
    *out = h_value;
    return LEDGER_OK;

rollback:
    EXEC SQL ROLLBACK;
    return rc;
}
```

## SQL 실행 뒤 확인하는 값

| 값 | 의미 |
|---|---|
| `sqlca.sqlcode == 0` | 성공 |
| `sqlca.sqlcode == ECPG_NOT_FOUND` (100) | 조회, 수정, 삭제 대상이 한 건도 없음. 오류는 아니다 |
| `sqlca.sqlcode < 0` | 오류 |
| `sqlca.sqlerrd[2]` | 영향받은 행 수 (INSERT, UPDATE, DELETE) |
| `sqlca.sqlstate` | 오류 종류 5글자. 끝에 `'\0'`이 없으니 `ledger_db_sqlstate_is("23505")`로 비교한다 |
| `sqlca.sqlerrm.sqlerrmc` | 오류 메시지. `ledger_db_log_error`가 찍어 준다 |

자주 보는 SQLSTATE: `23505` 유일성 위반, `23514` CHECK 위반, `23503` 외래키 위반, `40P01` 데드락.

## 트랜잭션

- `BEGIN`을 쓰지 않는다. 첫 SQL 문장에서 트랜잭션이 자동으로 시작된다.
- `EXEC SQL COMMIT`을 하지 않으면 반영되지 않는다. 접속을 끊으면 롤백된다.
- 트랜잭션 안에서 오류가 한 번 나면, `ROLLBACK`하기 전까지 그 뒤의 모든 문장이 실패한다
  (SQLSTATE `25P02`). 오류를 보면 바로 `goto rollback`으로 나가야 하는 이유다.
- 조회만 하는 함수도 끝에서 `COMMIT`이나 `ROLLBACK`으로 트랜잭션을 닫는다.

## 자주 걸리는 함정

- **NULL**: NULL이 올 수 있는 컬럼을 그냥 받으면 오류(sqlcode -213)가 난다.
  표시 변수를 같이 쓴다: `short h_ind;` 선언 후 `INTO :h_value :h_ind`, `h_ind < 0`이면 NULL.
- **CHAR(n)**: 남는 자리가 공백으로 채워져서 온다. `CHAR(12)`에 `'abc'`를 넣으면 `'abc         '`로 읽힌다.
- **문자 배열 크기**: 컬럼 길이 + 1. 한글은 UTF-8에서 글자당 3바이트다.
- **`.pgc`에 clang-format 금지**: `EXEC SQL` 문장이 깨진다.
- **컴파일 오류의 줄 번호**: 생성된 `.c`가 아니라 원본 `.pgc`의 줄 번호로 나온다.
- **fork**: 자식 프로세스는 부모의 접속을 쓰면 안 된다. 자식에서 `ledger_connect()`를 새로 부른다.

## 자주 쓰는 문장

```c
/* 한 건 조회 */
EXEC SQL SELECT col_a, col_b INTO :h_a, :h_b FROM some_table WHERE id = :h_id;

/* 넣기 */
EXEC SQL INSERT INTO some_table (col_a, col_b) VALUES (:h_a, :h_b);

/* 넣으면서 DB가 만든 값 받기 */
EXEC SQL INSERT INTO some_table (col_a) VALUES (:h_a) RETURNING id INTO :h_id;

/* 고치기. 고쳐진 행 수는 sqlca.sqlerrd[2] */
EXEC SQL UPDATE some_table SET col_a = :h_a WHERE id = :h_id;
```

여러 건을 읽으려면 커서(`DECLARE ... CURSOR`, `OPEN`, `FETCH`, `CLOSE`)를 쓴다.
자세한 문법은 PostgreSQL 공식 문서의 ECPG 장을 본다.

## C가 처음이라면

- 빌드는 항상 ASan + UBSan이 켜진 상태다. 배열 범위를 넘거나 초기화 안 한 값을 쓰면
  테스트가 바로 죽으면서 위치를 알려 준다. 이 출력을 읽는 데 먼저 익숙해진다.
- 경고는 전부 오류로 처리된다 (`-Wall -Wextra -Werror`). 경고를 끄지 말고 원인을 고친다.
- 문자열은 `strcpy`, `strcat`, `sprintf` 대신 `snprintf`만 쓴다.
- 함수가 값을 여러 개 돌려줘야 하면 반환값은 결과 코드로 쓰고, 값은 포인터 인자로 내보낸다.
