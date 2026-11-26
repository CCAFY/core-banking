#ifndef LEDGER_LEDGER_H
#define LEDGER_LEDGER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 결과 코드. 0 이상은 정상, 음수는 오류.
 * 지금은 접속에 필요한 것만 있다. 업무 결과 코드는 팀이 설계하면서 추가한다.
 * (docs/design-questions.md의 "8. 결과 코드와 함수 규칙")
 */
typedef enum {
    LEDGER_OK = 0,
    LEDGER_ERR_INVALID = -1, /* 입력값 오류 */
    LEDGER_ERR_DB = -9       /* DB 오류. 트랜잭션은 롤백된 상태다 */
} ledger_rc;

const char *ledger_rc_str(ledger_rc rc);

/*
 * DB 접속. 프로세스당 접속 하나를 쓴다. fork한 자식은 자기 접속을 새로 연다.
 * 환경변수: LEDGER_DB_HOST, LEDGER_DB_PORT, LEDGER_DB_NAME, LEDGER_DB_USER, LEDGER_DB_PASSWORD
 */
ledger_rc ledger_connect(void);
void ledger_disconnect(void);

/*
 * 접속 확인용. DB에서 숫자 1을 읽어 *out에 넣는다.
 * ECPG 함수를 어떻게 짜는지 보여주는 예시이기도 하다 (src/db.pgc).
 */
ledger_rc ledger_db_ping(int *out);

#ifdef __cplusplus
}
#endif

#endif
