// 환경이 제대로 붙었는지만 확인하는 테스트. 원장 규칙은 아직 검증하지 않는다.
// 테스트를 어떻게 쓰는지 보여주는 예시이기도 하다. 원장 테스트는 새 파일로 만든다.
#include "support/db_fixture.h"

// DB가 필요 없는 테스트는 TEST로 쓴다.
TEST(Smoke, ResultCodeHasName)
{
    EXPECT_STREQ(ledger_rc_str(LEDGER_OK), "OK");
    EXPECT_STREQ(ledger_rc_str(LEDGER_ERR_DB), "ERR_DB");
}

// DB가 필요한 테스트는 LedgerDbTest를 상속하고 TEST_F로 쓴다.
class SmokeDbTest : public LedgerDbTest {};

TEST_F(SmokeDbTest, LibraryCanQueryThroughEcpg)
{
    int value = 0;

    EXPECT_EQ(ledger_db_ping(&value), LEDGER_OK);
    EXPECT_EQ(value, 1);
    EXPECT_EQ(ledger_db_ping(nullptr), LEDGER_ERR_INVALID);
}

TEST_F(SmokeDbTest, VerifierSeesWhatTestsWrite)
{
    Exec("CREATE TABLE smoke_check (n INT PRIMARY KEY)");
    Exec("INSERT INTO smoke_check VALUES (1), (2), (3)");

    EXPECT_EQ(QueryInt("SELECT count(*) FROM smoke_check"), 3);
    EXPECT_TRUE(ExecFails("INSERT INTO smoke_check VALUES (1)")); // 중복 키라 거절돼야 한다
}

TEST_F(SmokeDbTest, EachTestStartsFromFreshSchema)
{
    // 앞 테스트가 만든 테이블은 남아 있지 않아야 한다.
    EXPECT_EQ(QueryInt("SELECT count(*) FROM pg_tables WHERE tablename = 'smoke_check'"), 0);
}

TEST_F(SmokeDbTest, ChildProcessesEachGetTheirOwnConnection)
{
    RunInChildren(4, [](int) {
        int value = 0;
        return (ledger_db_ping(&value) == LEDGER_OK && value == 1) ? 0 : 1;
    });

    EXPECT_EQ(QueryInt("SELECT 1"), 1);
}
