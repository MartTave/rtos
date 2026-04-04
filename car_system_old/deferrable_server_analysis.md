# Deferrable Server Dimensioning Analysis

## Given Data

### Sporadic Tasks (Aperiodic)
| Task | WCET |
|------|------|
| PD (Presence Detection) | 50ms |
| SPD (Search Peripheral Devices) | 200ms |

### Periodic Tasks (for reference)
| Task | WCET | Period |
|------|------|--------|
| Engine | 10ms | 50ms |
| Display | 15ms | 125ms |
| Tire | 10ms | 200ms |
| Rain | 25ms | 250ms |

---

## Step 1: Compute Capacity (Qs)

**Rule**: Qs must be ≥ maximum WCET of any sporadic task

```
Qs ≥ max(50ms, 200ms) = 200ms
```

With margin: **Qs = 250ms**

---

## Step 2: Compute Period (Ts)

**Rule**: Ts is typically chosen as a multiple of the periodic task periods (commonly the hyperperiod)

**Calculate hyperperiod of periodic tasks:**
```
LCM(50, 125, 200, 250) = 1000ms
```

This aligns with the sporadic task generator's major cycle (1000ms).

**Ts = 1000ms**

---

## Step 3: Verify System Utilization

### Periodic Tasks Utilization
| Task | WCET | Period | Utilization |
|------|------|--------|-------------|
| Engine | 10ms | 50ms | 10/50 = 0.20 |
| Display | 15ms | 125ms | 15/125 = 0.12 |
| Tire | 10ms | 200ms | 10/200 = 0.05 |
| Rain | 25ms | 250ms | 25/250 = 0.10 |
| **Total periodic** | | | **0.47** |

### Deferrable Server Utilization
```
U_s = Qs / Ts = 250ms / 1000ms = 0.25
```

### Total System Utilization
```
U_total = 0.47 + 0.25 = 0.72 < 1.0 ✓
```

**Result**: System is schedulable (utilization < 100%)

---

## Summary

| Parameter | Value | Justification |
|-----------|-------|----------------|
| **Qs (Capacity)** | 250ms | ≥ max sporadic WCET (200ms) + margin |
| **Ts (Period)** | 1000ms | Matches hyperperiod and generator cycle |