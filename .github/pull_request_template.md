## 무엇을 바꿨나


## 관련 ADR

설계 결정이 들어간 PR이면 ADR 번호를 적는다. 없으면 "없음".

## 확인

- [ ] `scripts/test.sh` 전체 통과
- [ ] 새 거래나 규칙에는 테스트를 추가했다
- [ ] 금액에 `float`/`double`을 쓰지 않았다
- [ ] SQL 문장마다 `sqlca.sqlcode`를 확인하고, 오류 경로는 전부 `ROLLBACK`으로 끝난다
- [ ] 스키마나 규칙을 바꿨다면 `docs/invariants.md`와 `docs/glossary.md`를 같이 고쳤다
