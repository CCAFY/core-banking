#include "support/db_fixture.h"

#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

namespace {

std::string EnvOr(const char *name, const char *fallback)
{
    const char *value = std::getenv(name);
    return (value != nullptr && value[0] != '\0') ? value : fallback;
}

std::string ConnInfo()
{
    return "host=" + EnvOr("LEDGER_DB_HOST", "localhost") +
           " port=" + EnvOr("LEDGER_DB_PORT", "5432") +
           " dbname=" + EnvOr("LEDGER_DB_NAME", "ledger") +
           " user=" + EnvOr("LEDGER_DB_USER", "ledger") +
           " password=" + EnvOr("LEDGER_DB_PASSWORD", "ledger") +
           " options='-c client_min_messages=warning'";
}

bool Succeeded(ExecStatusType status)
{
    return status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK || status == PGRES_EMPTY_QUERY;
}

} // namespace

void LedgerDbTest::OpenVerifier()
{
    verifier_ = PQconnectdb(ConnInfo().c_str());
    ASSERT_EQ(PQstatus(verifier_), CONNECTION_OK) << PQerrorMessage(verifier_);
}

void LedgerDbTest::CloseVerifier()
{
    if (verifier_ != nullptr) {
        PQfinish(verifier_);
        verifier_ = nullptr;
    }
}

void LedgerDbTest::ResetSchema()
{
    std::ifstream file(LEDGER_SCHEMA_FILE);
    ASSERT_TRUE(file.is_open()) << "cannot open " << LEDGER_SCHEMA_FILE;
    std::stringstream schema;
    schema << file.rdbuf();

    Exec("DROP SCHEMA public CASCADE");
    Exec("CREATE SCHEMA public");
    Exec(schema.str());
}

void LedgerDbTest::SetUp()
{
    OpenVerifier();
    ResetSchema();
    ASSERT_EQ(ledger_connect(), LEDGER_OK);
}

void LedgerDbTest::TearDown()
{
    ledger_disconnect();
    CloseVerifier();
}

long long LedgerDbTest::QueryInt(const std::string &sql)
{
    PGresult *res = PQexec(verifier_, sql.c_str());
    long long value = -1;

    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) == 1 && PQnfields(res) == 1) {
        value = std::strtoll(PQgetvalue(res, 0, 0), nullptr, 10);
    } else {
        ADD_FAILURE() << "query failed: " << sql << "\n" << PQerrorMessage(verifier_);
    }
    PQclear(res);
    return value;
}

void LedgerDbTest::Exec(const std::string &sql)
{
    PGresult *res = PQexec(verifier_, sql.c_str());

    EXPECT_TRUE(Succeeded(PQresultStatus(res))) << sql << "\n" << PQerrorMessage(verifier_);
    PQclear(res);
}

bool LedgerDbTest::ExecFails(const std::string &sql)
{
    PGresult *res = PQexec(verifier_, sql.c_str());
    bool failed = !Succeeded(PQresultStatus(res));

    PQclear(res);
    return failed;
}

void LedgerDbTest::RunInChildren(int n, const std::function<int(int)> &work)
{
    // 접속을 자식과 공유하지 않도록 fork 전에 전부 닫는다.
    ledger_disconnect();
    CloseVerifier();
    std::fflush(nullptr);

    std::vector<pid_t> children;
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        ASSERT_GE(pid, 0);
        if (pid == 0) {
            int status = 1;
            if (ledger_connect() == LEDGER_OK) {
                status = work(i);
                ledger_disconnect();
            }
            _exit(status);
        }
        children.push_back(pid);
    }
    for (pid_t pid : children) {
        int status = 0;
        ASSERT_EQ(waitpid(pid, &status, 0), pid);
        EXPECT_TRUE(WIFEXITED(status) && WEXITSTATUS(status) == 0) << "child " << pid << " failed";
    }

    OpenVerifier();
    ASSERT_EQ(ledger_connect(), LEDGER_OK);
}
