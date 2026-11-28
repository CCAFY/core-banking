# 사전 지식: 그림으로 보는 원장

`docs/glossary.md`의 용어를 그림 6장으로 정리했다.
설계 질문(`docs/design-questions.md`)의 답은 넣지 않았다. 팀이 정할 자리는 노란 점선 상자로 비워 두었다.

## 범례

```mermaid
%%{init: {"theme": "base", "themeVariables": {"fontSize": "14px", "lineColor": "#94A3B8", "textColor": "#334155", "edgeLabelBackground": "#FFFFFF", "clusterBkg": "#F8FAFC", "clusterBorder": "#E2E8F0", "titleColor": "#475569"}, "flowchart": {"curve": "linear", "nodeSpacing": 24, "rankSpacing": 32}}}%%
flowchart LR
    classDef input  fill:#4F46E5,stroke:#4338CA,stroke-width:1.5px,color:#FFFFFF
    classDef step   fill:#EEF2FF,stroke:#6366F1,stroke-width:1.5px,color:#1E1B4B
    classDef value  fill:#FFFFFF,stroke:#CBD5E1,stroke-width:1.5px,color:#334155
    classDef debit  fill:#F0FDFA,stroke:#14B8A6,stroke-width:1.5px,color:#134E4A
    classDef credit fill:#FFF7ED,stroke:#F97316,stroke-width:1.5px,color:#7C2D12
    classDef ok     fill:#F0FDF4,stroke:#22C55E,stroke-width:1.5px,color:#14532D
    classDef danger fill:#FEF2F2,stroke:#EF4444,stroke-width:1.5px,color:#7F1D1D
    classDef open   fill:#FEFCE8,stroke:#CA8A04,stroke-width:1.5px,stroke-dasharray:5 3,color:#713F12

    L1(["입력"]):::input ~~~ L2["처리"]:::step ~~~ L3["값"]:::value ~~~ L4["차변"]:::debit
    L5["대변"]:::credit ~~~ L6["일치"]:::ok ~~~ L7["금지 · 불일치"]:::danger ~~~ L8("팀이 정할 것"):::open
```

---

## 1. 큰 그림: 거래 한 건이 지나가는 길

```mermaid
%%{init: {"theme": "base", "themeVariables": {"fontSize": "14px", "lineColor": "#94A3B8", "textColor": "#334155", "edgeLabelBackground": "#FFFFFF", "clusterBkg": "#F8FAFC", "clusterBorder": "#E2E8F0", "titleColor": "#475569"}, "flowchart": {"curve": "linear", "nodeSpacing": 32, "rankSpacing": 40}}}%%
flowchart TB
    classDef input  fill:#4F46E5,stroke:#4338CA,stroke-width:1.5px,color:#FFFFFF
    classDef step   fill:#EEF2FF,stroke:#6366F1,stroke-width:1.5px,color:#1E1B4B
    classDef ok     fill:#F0FDF4,stroke:#22C55E,stroke-width:1.5px,color:#14532D
    classDef open   fill:#FEFCE8,stroke:#CA8A04,stroke-width:1.5px,stroke-dasharray:5 3,color:#713F12

    REQ(["거래 요청 · 추적번호 포함"]):::input
    DUP{"이 추적번호를<br/>이미 반영했나?"}:::step
    S2["분개<br/>차변 합 = 대변 합"]:::step
    S3["원장에 추가<br/>수정 · 삭제 없음"]:::step
    S4["거래 후 잔액 확정"]:::step
    ANS("무엇을 돌려주나? §3"):::open

    REQ --> DUP
    DUP -- "아니요" --> S2 --> S3 --> S4
    DUP -- "예" --> ANS

    subgraph EOD ["하루가 끝나면"]
        direction LR
        E1["일마감<br/>영업일을 넘긴다"]:::step --> E2["대사<br/>두 기록을 맞춰 본다"]:::ok
    end

    S4 -.-> EOD
```

---

## 2. 복식부기: 입금 10,000원

```mermaid
%%{init: {"theme": "base", "themeVariables": {"fontSize": "14px", "lineColor": "#94A3B8", "textColor": "#334155", "edgeLabelBackground": "#FFFFFF", "clusterBkg": "#F8FAFC", "clusterBorder": "#E2E8F0", "titleColor": "#475569"}, "flowchart": {"curve": "linear", "nodeSpacing": 28, "rankSpacing": 48}}}%%
flowchart LR
    classDef input  fill:#4F46E5,stroke:#4338CA,stroke-width:1.5px,color:#FFFFFF
    classDef value  fill:#FFFFFF,stroke:#CBD5E1,stroke-width:1.5px,color:#94A3B8
    classDef debit  fill:#F0FDFA,stroke:#14B8A6,stroke-width:1.5px,color:#134E4A
    classDef credit fill:#FFF7ED,stroke:#F97316,stroke-width:1.5px,color:#7C2D12
    classDef ok     fill:#F0FDF4,stroke:#22C55E,stroke-width:1.5px,color:#14532D

    EV(["현금 10,000원 입금"]):::input

    subgraph CASH ["현금 · 자산"]
        direction LR
        CD["차변<br/><b>+10,000</b>"]:::debit ~~~ CC["대변<br/>·"]:::value
    end

    subgraph DEP ["보통예금 · 부채"]
        direction LR
        DD["차변<br/>·"]:::value ~~~ DC["대변<br/><b>+10,000</b>"]:::credit
    end

    CHK{{"차변 합 = 대변 합<br/>불변식 3"}}:::ok

    EV -- "자산 증가" --> CASH
    EV -- "부채 증가" --> DEP
    CASH --> CHK
    DEP --> CHK
```

- 자산은 차변에서 늘고, 부채는 대변에서 는다.
- 예금이 부채인 이유: 은행은 이 돈을 고객이 달라고 할 때 돌려줘야 한다.
- 출금과 이체의 분개는 직접 그려 볼 것 (`design-questions.md` §4, §5).

---

## 3. 취소: 지우지 않고 반대로 한 줄 더

```mermaid
%%{init: {"theme": "base", "themeVariables": {"fontSize": "14px", "lineColor": "#94A3B8", "textColor": "#334155", "edgeLabelBackground": "#FFFFFF", "clusterBkg": "#F8FAFC", "clusterBorder": "#E2E8F0", "titleColor": "#475569"}, "flowchart": {"curve": "linear", "nodeSpacing": 28, "rankSpacing": 40}}}%%
flowchart LR
    classDef step   fill:#EEF2FF,stroke:#6366F1,stroke-width:1.5px,color:#1E1B4B
    classDef danger fill:#FEF2F2,stroke:#EF4444,stroke-width:1.5px,stroke-dasharray:5 3,color:#7F1D1D
    classDef open   fill:#FEFCE8,stroke:#CA8A04,stroke-width:1.5px,stroke-dasharray:5 3,color:#713F12

    subgraph LEDGER ["원장 · 추가만 가능"]
        direction TB
        R1["<b>1번 · 입금</b><br/>차변 현금 10,000<br/>대변 보통예금 10,000<br/>잔액 10,000"]:::step
        R2["<b>2번 · 1번 취소</b><br/>차변 보통예금 10,000<br/>대변 현금 10,000<br/>잔액 0"]:::step
        R1 --> R2
    end

    subgraph NO ["하지 않는 것"]
        direction TB
        X1["DELETE 1번<br/>기록이 사라진다"]:::danger
        X2["UPDATE 1번<br/>무슨 일이 있었는지 모른다"]:::danger
    end

    R2 -. "원거래 참조" .-> LINK("연결 방법은? §6"):::open
    LEDGER ~~~ NO
```

- 2번은 1번의 차변과 대변을 서로 바꾼 분개다. 이것이 역분개다.
- 1번은 그대로 남는다. 그래서 무슨 일이 있었는지 끝까지 추적할 수 있다. 불변식 4가 이 규칙이다.

---

## 4. 멱등성: 응답을 못 받은 쪽이 다시 보낼 때

```mermaid
%%{init: {"theme": "base", "themeVariables": {"fontSize": "14px", "actorBkg": "#FFFFFF", "actorBorder": "#6366F1", "actorTextColor": "#1E1B4B", "actorLineColor": "#CBD5E1", "signalColor": "#64748B", "signalTextColor": "#334155", "noteBkgColor": "#FFFFFF", "noteBorderColor": "#CBD5E1", "noteTextColor": "#334155", "sequenceNumberColor": "#FFFFFF"}}}%%
sequenceDiagram
    autonumber
    participant C as 클라이언트
    participant L as 원장 서비스
    participant D as DB

    rect rgb(248, 250, 252)
        C->>L: 입금 · T-001 · 10,000원
        L->>D: T-001 기록 + 분개 추가
        D-->>L: OK
    end

    rect rgb(254, 242, 242)
        L--xC: 응답 유실
        Note over C: 돈이 들어갔는지 모른다
    end

    rect rgb(238, 242, 255)
        C->>L: 같은 요청 재전송 · T-001
        L->>D: T-001을 이미 봤나?
        D-->>L: 봤다
        L-->>C: 오류인가, 첫 결과인가? (§3)
    end
```

- 돈은 이미 들어갔는데, 클라이언트는 그 사실을 모른다. 그래서 같은 요청을 다시 보낸다.
- 추적번호가 없으면 10,000원이 두 번 들어간다. 불변식 5가 이것을 막는 규칙이다.

---

## 5. 영업일: 시스템 날짜와 다를 수 있다

```mermaid
%%{init: {"theme": "base", "themeVariables": {"fontSize": "14px", "lineColor": "#94A3B8", "textColor": "#334155", "edgeLabelBackground": "#FFFFFF", "clusterBkg": "#F8FAFC", "clusterBorder": "#E2E8F0", "titleColor": "#475569"}, "flowchart": {"curve": "linear", "nodeSpacing": 28, "rankSpacing": 28}}}%%
flowchart LR
    classDef input  fill:#4F46E5,stroke:#4338CA,stroke-width:1.5px,color:#FFFFFF
    classDef step   fill:#EEF2FF,stroke:#6366F1,stroke-width:1.5px,color:#1E1B4B
    classDef value  fill:#FFFFFF,stroke:#CBD5E1,stroke-width:1.5px,color:#334155
    classDef open   fill:#FEFCE8,stroke:#CA8A04,stroke-width:1.5px,stroke-dasharray:5 3,color:#713F12

    T1["<b>23:50</b><br/>시스템 11/30<br/>영업일 11/30"]:::value
    T2["<b>00:00 자정</b><br/>시스템 12/01<br/>영업일 11/30"]:::value
    T3(["<b>00:05 입금</b><br/>시스템 12/01<br/>거래일자 11/30"]):::input
    T4["<b>00:15 일마감</b><br/>영업일을 넘긴다"]:::step
    T5["<b>00:20</b><br/>시스템 12/01<br/>영업일 12/01"]:::value
    Q("마감 도중 거래는? §7"):::open

    T1 --> T2 --> T3 --> T4 --> T5
    T4 -.- Q
```

00:05 입금의 거래일자는 11/30이다. 시스템 날짜는 12/01이지만, 일마감 전이므로 영업일은 아직 11/30이다. 불변식 8이 이 규칙이다.

---

## 6. 대사: 두 기록을 맞춰 본다

```mermaid
%%{init: {"theme": "base", "themeVariables": {"fontSize": "14px", "lineColor": "#94A3B8", "textColor": "#334155", "edgeLabelBackground": "#FFFFFF", "clusterBkg": "#F8FAFC", "clusterBorder": "#E2E8F0", "titleColor": "#475569"}, "flowchart": {"curve": "linear", "nodeSpacing": 20, "rankSpacing": 80}}}%%
flowchart LR
    classDef value  fill:#FFFFFF,stroke:#CBD5E1,stroke-width:1.5px,color:#334155
    classDef danger fill:#FEF2F2,stroke:#EF4444,stroke-width:1.5px,color:#7F1D1D

    subgraph A ["계좌에 적힌 잔액"]
        direction TB
        a1["ACC-001 · 50,000"]:::value
        a2["ACC-002 · 30,000"]:::danger
        a3["ACC-003 · 0"]:::value
    end

    subgraph B ["거래 기록으로 다시 계산"]
        direction TB
        b1["ACC-001 · 50,000"]:::value
        b2["ACC-002 · 20,000"]:::danger
        b3["ACC-003 · 0"]:::value
    end

    a1 -- "일치" --- b1
    a2 -- "불일치" --- b2
    a3 -- "일치" --- b3

    linkStyle 0 stroke:#22C55E,stroke-width:1.5px
    linkStyle 1 stroke:#EF4444,stroke-width:2.5px
    linkStyle 2 stroke:#22C55E,stroke-width:1.5px
```

대사는 불일치가 난 계좌를 찾아내는 작업이다. 두 값이 다르면 어딘가에서 장부가 깨진 것이다. 불변식 6이 이 규칙이다.

---

## 그림 고치는 법

- 그림은 모두 Mermaid 코드 블록이다. GitHub이 자동으로 그림으로 바꿔 보여 준다.
- 고친 결과는 https://mermaid.live 에 코드 블록 내용을 붙여 넣어 미리 볼 수 있다.
- 색은 범례의 `classDef` 8개만 쓴다. 새 색을 만들지 말고, 뜻이 맞는 역할을 골라 붙인다.
  - 진하게 칠한 `input`은 그림 하나에 하나만 쓴다. 그림의 출발점이다.
  - `danger`와 `open`은 각각 금지·불일치와 미결정에만 쓴다.
- 각 블록의 첫 줄(`%%{init: ...}%%`)은 그대로 복사해서 쓴다. 이 줄이 있어야 GitHub의 라이트 모드와 다크 모드에서 글자가 모두 보인다.
