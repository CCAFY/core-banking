#pragma once

#include <gtest/gtest.h>
#include <libpq-fe.h>

#include <functional>
#include <string>

#include "ledger/ledger.h"

// DB를 쓰는 테스트의 공통 바탕. TEST_F(내테스트, ...)에서 상속해서 쓴다.
//
// - 테스트마다 스키마를 통째로 지우고 db/schema.sql을 다시 적용한 상태에서 시작한다.
// - 원장 라이브러리 접속(ledger_connect)은 SetUp에서 열고 TearDown에서 닫는다.
// - 검증용 조회(QueryInt 등)는 원장 라이브러리를 거치지 않고 libpq로 DB를 직접 본다.
//   검증 대상과 같은 코드로 검증하면 같은 버그를 못 잡기 때문이다.
class LedgerDbTest : public ::testing::Test {
  protected:
    void SetUp() override;
    void TearDown() override;

    // 정수 하나를 돌려주는 조회. 예: QueryInt("SELECT count(*) FROM some_table")
    long long QueryInt(const std::string &sql);
    // 실행이 성공해야 하는 SQL
    void Exec(const std::string &sql);
    // 실행이 실패하면 true. "이 SQL은 DB가 거절해야 한다"를 확인할 때 쓴다.
    bool ExecFails(const std::string &sql);

    // 동시성 테스트용. 자식 프로세스 n개를 띄워 각각 work(자식번호)를 실행한다.
    // 자식마다 원장 접속을 따로 연다. work가 0을 돌려주면 성공이다.
    // 전부 끝난 뒤 부모의 접속을 다시 열어 주므로 이어서 QueryInt로 결과를 확인하면 된다.
    void RunInChildren(int n, const std::function<int(int)> &work);

  private:
    void OpenVerifier();
    void CloseVerifier();
    void ResetSchema();

    PGconn *verifier_ = nullptr;
};
