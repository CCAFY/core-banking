# ledger

은행 계정계의 핵심인 원장(ledger)을 C로 직접 만들어 보는 학습 프로젝트.
미들웨어 없이 원장 규칙 자체(복식부기, 동시성, 멱등성, 대사)를 정확하게 구현하는 데 집중한다.

| 역할 | 도구 |
|---|---|
| 업무 로직 | C11 + ECPG (C 안에 SQL을 직접 쓰는 임베디드 SQL) |
| DB | PostgreSQL 16 |
| 빌드, 테스트 | CMake + GoogleTest (테스트 코드만 C++) |
| 환경 | Docker Compose |

도구를 이렇게 고른 이유는 [docs/adr/0001-language-and-tools.md](docs/adr/0001-language-and-tools.md)에 있다.

## 지금 상태

**환경만 준비돼 있고, 원장 설계와 구현은 비어 있다.**

| 준비된 것 | 팀이 채울 것 |
|---|---|
| 빌드 (CMake + ECPG 연동) | 테이블 설계 (`db/schema.sql`) |
| DB와 개발용 컨테이너 (Docker Compose) | 거래 함수 (계좌 개설, 입금, 출금, 이체, 취소) |
| CI (ASan + UBSan 빌드, 전체 테스트) | 결과 코드와 함수 규칙 (`include/ledger/ledger.h`) |
| 테스트 도구 (스키마 초기화, 직접 조회, 다중 프로세스 실행) | 원장 테스트 |
| DB 접속 함수와 ECPG 예시 함수 하나 (`src/db.pgc`) | 설계 결정 기록 (`docs/adr/`) |

## 빠른 시작

Docker만 있으면 된다.

```sh
docker compose up -d db                        # PostgreSQL 실행
docker compose run --rm dev scripts/test.sh    # 빌드 + 테스트 (지금은 환경 확인용 5개)
```

## 어디서부터 시작하나

1. [docs/ecpg-guide.md](docs/ecpg-guide.md): Java/Spring에 익숙한 사람을 위한 C + ECPG 안내.
2. [docs/invariants.md](docs/invariants.md): 원장이 항상 지켜야 하는 규칙. 요구사항이다.
3. [docs/design-questions.md](docs/design-questions.md): 영역별로 팀이 정해야 하는 것. 답은 적혀 있지 않다.
4. 정한 내용은 [docs/adr/0000-template.md](docs/adr/0000-template.md)를 복사해 기록한다.

## 디렉터리

```
db/schema.sql            테이블 정의 (비어 있음)
include/ledger/ledger.h  공개 API
src/db.pgc               DB 접속, 공통 도우미, ECPG 예시 함수
cli/main.c               명령줄 도구 (지금은 ping만)
tests/support/           테스트 도구 (LedgerDbTest)
tests/test_smoke.cpp     환경 확인 테스트이자 테스트 작성 예시
docs/                    불변식, 설계 질문지, ECPG 안내, 용어사전, 결정 기록
```

## 파일을 추가할 때

| 추가하는 것 | 등록하는 곳 |
|---|---|
| SQL이 들어가는 소스 `src/xxx.pgc` | `CMakeLists.txt`의 `ecpg_precompile(...)` |
| SQL이 없는 소스 `src/xxx.c` | `CMakeLists.txt`의 `add_library(ledger ...)` |
| 테스트 `tests/test_xxx.cpp` | `tests/CMakeLists.txt`의 `add_executable(ledger_tests ...)` |
| 테이블 | `db/schema.sql` (테스트가 매번 이 파일로 DB를 새로 만든다) |

## 작업 규칙

- `main`에 직접 push하지 않는다. 브랜치를 만들고 PR을 올려 1명 이상 승인을 받는다.
- PR은 CI를 통과해야 한다.
- 설계 결정은 코드보다 먼저 ADR로 올려 팀이 합의한다.
- `.pgc`에는 clang-format을 돌리지 않는다 (`EXEC SQL` 문장이 깨진다). `.c`, `.h`, `.cpp`만 돌린다.

## Docker 없이 빌드하기

Ubuntu 24.04 기준으로 아래 패키지가 필요하다.

```sh
sudo apt-get install build-essential cmake libecpg-dev libpq-dev libgtest-dev postgresql-client
```

접속 정보는 환경변수로 바꾼다 (기본값은 전부 `ledger`, 호스트 `localhost`, 포트 `5432`).
`LEDGER_DB_HOST`, `LEDGER_DB_PORT`, `LEDGER_DB_NAME`, `LEDGER_DB_USER`, `LEDGER_DB_PASSWORD`

```sh
scripts/db-reset.sh   # 스키마 적용 (기존 데이터 삭제)
scripts/test.sh       # 빌드 + 테스트
```
