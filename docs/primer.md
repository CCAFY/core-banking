# 사전 지식: 그림으로 보는 원장

`docs/glossary.md`의 용어를 그림 6장으로 정리했다.
설계 질문(`docs/design-questions.md`)의 답은 넣지 않았다. 팀이 정할 자리는 노란색 `?` 상자로 비워 두었다.

## 색 규칙

모든 그림에서 같은 색은 같은 뜻이다.

```mermaid
%%{init: {"theme": "base", "themeVariables": {"lineColor": "#64748B", "edgeLabelBackground": "#F8FAFC", "clusterBkg": "#F8FAFC", "clusterBorder": "#94A3B8", "titleColor": "#0F172A", "textColor": "#0F172A"}}}%%
flowchart LR
    classDef event  fill:#C026D3,stroke:#A21CAF,color:#FFFFFF
    classDef step   fill:#2563EB,stroke:#1D4ED8,color:#FFFFFF
    classDef idem   fill:#7C3AED,stroke:#6D28D9,color:#FFFFFF
    classDef debit  fill:#0D9488,stroke:#0F766E,color:#FFFFFF
    classDef credit fill:#EA580C,stroke:#C2410C,color:#FFFFFF
    classDef ok     fill:#16A34A,stroke:#15803D,color:#FFFFFF
    classDef danger fill:#DC2626,stroke:#B91C1C,color:#FFFFFF
    classDef decide fill:#FACC15,stroke:#CA8A04,color:#1F2937
    classDef note   fill:#E2E8F0,stroke:#94A3B8,color:#1E293B

    L1(["들어온 거래"]):::event
    L2["처리 단계"]:::step
    L3["멱등성"]:::idem
    L4["차변"]:::debit
    L5["대변"]:::credit
    L6["일치 · 검증"]:::ok
    L7["금지 · 불일치"]:::danger
    L8{{"팀이 정할 것 ?"}}:::decide
    L9["값 · 설명"]:::note

    L1 ~~~ L2 ~~~ L3
    L4 ~~~ L5 ~~~ L6
    L7 ~~~ L8 ~~~ L9
```

---

## 1. 큰 그림: 거래 한 건이 지나가는 길

```mermaid
%%{init: {"theme": "base", "themeVariables": {"lineColor": "#64748B", "edgeLabelBackground": "#F8FAFC", "clusterBkg": "#F8FAFC", "clusterBorder": "#94A3B8", "titleColor": "#0F172A", "textColor": "#0F172A"}}}%%
flowchart TD
    classDef event  fill:#C026D3,stroke:#A21CAF,color:#FFFFFF
    classDef step   fill:#2563EB,stroke:#1D4ED8,color:#FFFFFF
    classDef idem   fill:#7C3AED,stroke:#6D28D9,color:#FFFFFF
    classDef ok     fill:#16A34A,stroke:#15803D,color:#FFFFFF
    classDef decide fill:#FACC15,stroke:#CA8A04,color:#1F2937

    REQ(["거래 요청<br/>입금 · 출금 · 이체 · 취소<br/>추적번호를 달고 온다"]):::event
    S1["1 · 중복 확인<br/>이 추적번호를 이미 반영했나?"]:::idem
    DUP{{"이미 반영한 요청에는<br/>무엇을 돌려주나? (§3)"}}:::decide
    S2["2 · 분개<br/>차변 합 = 대변 합"]:::step
    S3["3 · 원장에 추가<br/>추가만 한다<br/>고치지도 지우지도 않는다"]:::step
    S4["4 · 거래 후 잔액이 정해진다"]:::step

    REQ --> S1
    S1 -- "처음 본 요청" --> S2
    S1 -- "이미 본 요청" --> DUP
    S2 --> S3 --> S4

    subgraph EOD ["하루가 끝나면"]
        direction LR
        E1["일마감<br/>영업일을 다음 날로 넘긴다"]:::step
        E2["대사<br/>두 기록이 맞는지 맞춰 본다"]:::ok
        E1 --> E2
    end

    S4 -.-> EOD
```

---

## 2. 복식부기: 입금 10,000원

```mermaid
%%{init: {"theme": "base", "themeVariables": {"lineColor": "#64748B", "edgeLabelBackground": "#F8FAFC", "clusterBkg": "#F8FAFC", "clusterBorder": "#94A3B8", "titleColor": "#0F172A", "textColor": "#0F172A"}}}%%
flowchart LR
    classDef event  fill:#C026D3,stroke:#A21CAF,color:#FFFFFF
    classDef debit  fill:#0D9488,stroke:#0F766E,color:#FFFFFF
    classDef credit fill:#EA580C,stroke:#C2410C,color:#FFFFFF
    classDef ok     fill:#16A34A,stroke:#15803D,color:#FFFFFF
    classDef note   fill:#E2E8F0,stroke:#94A3B8,color:#1E293B

    EV(["고객이 현금<br/>10,000원 입금"]):::event

    subgraph CASH ["현금 · 자산 · 차변에서 는다"]
        direction LR
        CD["차변<br/>+10,000"]:::debit
        CC["대변<br/>　"]:::note
        CD ~~~ CC
    end

    subgraph DEP ["보통예금 · 부채 · 대변에서 는다"]
        direction LR
        DD["차변<br/>　"]:::note
        DC["대변<br/>+10,000"]:::credit
        DD ~~~ DC
    end

    CHK{{"차변 합 10,000 = 대변 합 10,000<br/>불변식 3"}}:::ok

    EV -- "금고에 돈이 늘었다" --> CASH
    EV -- "고객에게 돌려줄 돈이 늘었다" --> DEP
    CASH --> CHK
    DEP --> CHK
```

예금이 부채인 이유: 은행은 이 돈을 고객이 달라고 할 때 돌려줘야 한다.

출금과 이체의 분개는 직접 그려 볼 것 (`design-questions.md` §4, §5).

---

## 3. 취소: 지우지 않고 반대로 한 줄 더

```mermaid
%%{init: {"theme": "base", "themeVariables": {"lineColor": "#64748B", "edgeLabelBackground": "#F8FAFC", "clusterBkg": "#F8FAFC", "clusterBorder": "#94A3B8", "titleColor": "#0F172A", "textColor": "#0F172A"}}}%%
flowchart LR
    classDef step   fill:#2563EB,stroke:#1D4ED8,color:#FFFFFF
    classDef danger fill:#DC2626,stroke:#B91C1C,color:#FFFFFF
    classDef decide fill:#FACC15,stroke:#CA8A04,color:#1F2937

    subgraph LEDGER ["원장 · 새 줄을 덧붙이기만 한다"]
        direction TB
        R1["1번 · 입금<br/>차변 현금 10,000<br/>대변 보통예금 10,000<br/>잔액 10,000"]:::step
        R2["2번 · 1번 취소 (역분개)<br/>차변 보통예금 10,000<br/>대변 현금 10,000<br/>잔액 0"]:::step
        R1 --> R2
    end

    LINK{{"원거래와 어떻게 연결하나? (§6)"}}:::decide
    R2 -.- LINK -.-> R1

    X1["DELETE 1번<br/>기록이 사라진다"]:::danger
    X2["UPDATE 1번<br/>무슨 일이 있었는지 모른다"]:::danger
    X1 -. "하지 않는다" .-x R1
    X2 -. "하지 않는다" .-x R1
```

차변과 대변을 서로 바꾼 분개를 새로 넣는다. 이것이 역분개다. 불변식 4가 이 규칙이다.

---

## 4. 멱등성: 응답을 못 받은 쪽이 다시 보낼 때

```mermaid
%%{init: {"theme": "base", "themeVariables": {"actorBkg": "#2563EB", "actorBorder": "#1D4ED8", "actorTextColor": "#FFFFFF", "actorLineColor": "#94A3B8", "signalColor": "#475569", "signalTextColor": "#0F172A", "noteBkgColor": "#FEF9C3", "noteBorderColor": "#CA8A04", "noteTextColor": "#1F2937", "labelBoxBkgColor": "#7C3AED", "labelTextColor": "#FFFFFF"}}}%%
sequenceDiagram
    participant C as 클라이언트
    participant L as 원장 서비스
    participant D as DB

    rect rgb(219, 234, 254)
        Note over C,D: 1차 요청
        C->>L: 입금 (추적번호 T-001, 10,000원)
        L->>D: T-001 기록 + 분개 추가
        D-->>L: OK
    end

    rect rgb(254, 226, 226)
        L--xC: 응답 유실 (네트워크 끊김)
        Note over C: 돈이 들어갔는지 모른다
    end

    rect rgb(237, 233, 254)
        Note over C,D: 재요청 · 같은 추적번호
        C->>L: 입금 (추적번호 T-001, 10,000원)
        L->>D: T-001을 이미 봤나?
        D-->>L: 봤다
        L-->>C: ??? 오류인가, 첫 결과인가 (§3에서 결정)
    end
```

- 돈은 이미 들어갔는데, 클라이언트는 그 사실을 모른다. 그래서 같은 요청을 다시 보낸다.
- 추적번호가 없으면 10,000원이 두 번 들어간다. 불변식 5가 이것을 막는 규칙이다.
- 두 번째 요청에 무엇을 돌려줄지는 팀이 정한다 (`design-questions.md` §3).

---

## 5. 영업일: 시스템 날짜와 다를 수 있다

```mermaid
%%{init: {"theme": "base", "themeVariables": {"lineColor": "#64748B", "edgeLabelBackground": "#F8FAFC", "clusterBkg": "#F8FAFC", "clusterBorder": "#94A3B8", "titleColor": "#0F172A", "textColor": "#0F172A"}}}%%
flowchart LR
    classDef event  fill:#C026D3,stroke:#A21CAF,color:#FFFFFF
    classDef step   fill:#2563EB,stroke:#1D4ED8,color:#FFFFFF
    classDef decide fill:#FACC15,stroke:#CA8A04,color:#1F2937
    classDef note   fill:#E2E8F0,stroke:#94A3B8,color:#1E293B

    subgraph SYS ["시스템 날짜 · 자정에 바뀐다"]
        direction LR
        A1["23:50<br/>11/30"]:::note --> A2["00:00<br/>12/01"]:::note --> A3["00:05<br/>12/01"]:::note --> A4["00:20<br/>12/01"]:::note
    end

    subgraph BIZ ["영업일 · 일마감 때 바뀐다"]
        direction LR
        B1["11/30"]:::step --> B2["일마감"]:::step --> B3["12/01"]:::step
    end

    T(["00:05 입금"]):::event
    A3 -.- T
    T == "거래일자 = 영업일 11/30" ==> B1

    Q{{"일마감 도중에 들어온 거래는<br/>어느 날짜인가? (§7)"}}:::decide
    B2 -.- Q
```

00:05 입금의 거래일자는 11/30이다. 시스템 날짜는 12/01이지만, 일마감 전이므로 영업일은 아직 11/30이다. 불변식 8이 이 규칙이다.

---

## 6. 대사: 두 기록을 맞춰 본다

```mermaid
%%{init: {"theme": "base", "themeVariables": {"lineColor": "#64748B", "edgeLabelBackground": "#F8FAFC", "clusterBkg": "#F8FAFC", "clusterBorder": "#94A3B8", "titleColor": "#0F172A", "textColor": "#0F172A"}}}%%
flowchart LR
    classDef ok     fill:#16A34A,stroke:#15803D,color:#FFFFFF
    classDef danger fill:#DC2626,stroke:#B91C1C,color:#FFFFFF

    subgraph A ["기록 A · 계좌에 적힌 잔액"]
        direction TB
        a1["ACC-001<br/>50,000"]:::ok
        a2["ACC-002<br/>30,000"]:::danger
        a3["ACC-003<br/>0"]:::ok
    end

    subgraph B ["기록 B · 거래 기록으로 다시 계산"]
        direction TB
        b1["ACC-001<br/>50,000"]:::ok
        b2["ACC-002<br/>20,000"]:::danger
        b3["ACC-003<br/>0"]:::ok
    end

    a1 -- "일치" --- b1
    a2 -- "불일치 · 이것을 찾아낸다" --- b2
    a3 -- "일치" --- b3

    linkStyle 0 stroke:#16A34A,stroke-width:2px
    linkStyle 1 stroke:#DC2626,stroke-width:3px
    linkStyle 2 stroke:#16A34A,stroke-width:2px
```

두 값이 다르면 어딘가에서 장부가 깨진 것이다. 불변식 6이 이 규칙이다.

---

## 그림 고치는 법

- 그림은 모두 Mermaid 코드 블록이다. GitHub이 자동으로 그림으로 바꿔 보여 준다.
- 고친 결과는 https://mermaid.live 에 코드 블록 내용을 붙여 넣어 미리 볼 수 있다.
- 색은 위의 "색 규칙"에 있는 `classDef` 9개만 쓴다. 새 색을 만들지 말고, 뜻이 맞는 역할을 골라 붙인다.
- 각 블록의 첫 줄(`%%{init: ...}%%`)은 그대로 복사해서 쓴다. 이 줄이 있어야 GitHub의 라이트 모드와 다크 모드에서 글자가 모두 보인다.
