#!/usr/bin/env bash
# DB의 모든 테이블을 지우고 db/schema.sql을 다시 적용한다. 데이터가 전부 사라진다.
set -euo pipefail

cd "$(dirname "$0")/.."

export PGHOST="${LEDGER_DB_HOST:-localhost}"
export PGPORT="${LEDGER_DB_PORT:-5432}"
export PGDATABASE="${LEDGER_DB_NAME:-ledger}"
export PGUSER="${LEDGER_DB_USER:-ledger}"
export PGPASSWORD="${LEDGER_DB_PASSWORD:-ledger}"

psql -v ON_ERROR_STOP=1 -q -c "DROP SCHEMA public CASCADE" -c "CREATE SCHEMA public"
psql -v ON_ERROR_STOP=1 -q -f db/schema.sql
echo "schema reset: ${PGDATABASE}@${PGHOST}:${PGPORT}"
