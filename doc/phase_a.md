# Project Phase A: Rate Monotonic Scheduling with Zephyr RTOS

## 1. Feasibility Demonstration (RMA)
To demonstrate that the task set is feasible under the **Rate Monotonic Algorithm (RMA)**, we apply the Schedulability Test. For a set of $n$ tasks, the total utilization $U$ must satisfy:

$$U = \sum_{i=1}^{n} \frac{C_i}{T_i} \le n(2^{1/n} - 1)$$

*Where:*
* $C_i$ is the worst-case execution time.
* $T_i$ is the period.

### Simulation Results
[Insert Screenshot of Simulator Results Here]

---

## 2. Implementation Requirements

### Development Workflow
* **Branching:** All work is performed on the `phase_a` branch.
* **Merge Request:** Merging to `main` is handled via a Pull Request (PR) including a code reviewer.
* **Tagging:** The final source code is marked with the tag/release `PhaseA`.

### Task Specifications
* **Synchronization:** All tasks start with a phase of $0$ (simultaneous start for the first instance).
* **Execution:** Computing time is implemented via busy waiting (e.g., `zpp_lib::ThisThread::busy_wait()`).
* **Structure:** Tasks follow the implementation pattern defined in the *Scheduling of Periodic Tasks* codelab.

### Quality & Compliance
* **Coding Standard:** All code adheres to **MISRA C++:2023** guidelines.
* **CI/CD Pipeline:** The project includes a pipeline covering `pre-commit` and `misra_checker`.
* **Static Analysis:** All applicable rules are checked and fixes applied as per *Robust Development Methodologies*.

---

## 3. Dynamic Analysis & Validation

### SystemView Trace
[Insert Screenshot of SEGGER SystemView Dynamic Analysis Here]

> **Note:** The dynamic analysis results must align with the behavior modeled in the initial simulation.

### Statistical Analysis
The data collected via SEGGER SystemView is stored in `phase_a.csv` at the repository root. 

| Metric | Target Value | Measured Average | Tolerance | Status |
| :--- | :--- | :--- | :--- | :--- |
| **Period ($T$)** | TBD | TBD | $\pm 5\%$ | TBD |
| **Computation ($C$)** | TBD | TBD | $\pm 5\%$ | TBD |

---

## 4. Deliverables Checklist
- [ ] `README.md` containing RMA demonstration and screenshots.
- [ ] Simulator configuration file (`.json`) at the project root.
- [ ] Source code tagged as `PhaseA`.
- [ ] `phase_a.csv` containing raw SystemView data.
- [ ] Active CI/CD pipeline for MISRA compliance.
