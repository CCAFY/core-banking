/* 명령줄 도구. 지금은 접속 확인만 있다. 거래 명령은 기능을 만들면서 추가한다. */
#include <stdio.h>
#include <string.h>

#include "ledger/ledger.h"

static int usage(void)
{
    fprintf(stderr, "usage:\n"
                    "  ledger ping\n");
    return 2;
}

int main(int argc, char **argv)
{
    ledger_rc rc;
    int value = 0;

    if (argc != 2 || strcmp(argv[1], "ping") != 0) {
        return usage();
    }

    rc = ledger_connect();
    if (rc != LEDGER_OK) {
        fprintf(stderr, "connect failed: %s\n", ledger_rc_str(rc));
        return 1;
    }

    rc = ledger_db_ping(&value);
    printf("%s\n", ledger_rc_str(rc));

    ledger_disconnect();
    return rc >= 0 ? 0 : 1;
}
