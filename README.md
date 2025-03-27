# RISC-V Processor Simulator
By :-   1) Reeshabh Rajesh Kotecha          2023CS10018
        2) Vijay Balaji Narasimma Bharathi  2023CS51103

## Design Decisions
- Implemented a modular design by separating parsing, CSV printing, and processor logic into distinct files.
- Created a parent class Processor and two children classes Processor_Forwarding and Processor_NonForwarding. This has allowed our code to be extremely modular.
- Employed a pipeline structure with clear stages (IF, ID, EX, MEM, WB) to mimic processor execution.
- Used C++ bitfields for control signals to closely pack instruction details.
- Added comments at certain parts of the code and macros for better readability
- Provided both forwarding and non-forwarding implementations as separate classes.

## Known Issues
- The output printing can't handle bigger numbers. This design decision was done so that while printing smaller number we can look at more outputs at the standard output. If required for larger outputs, the code could be changed easily to handle them although it would sacrifice the readability in standard output.

## Sources and Consultation
- Official [RISC-V Specification](https://riscv.org/technical/specifications/).
- The RISC-V Instruction Set Manual Volume I: Unprivileged ISA (https://drive.google.com/file/d/1uviu1nH-tScFfgrovvFCrj7Omv8tFtkp/view)
- Computer Organization and Design: The Hardware/Software Interface: RISC-V Edition by David A. Patterson and John L. Hennessy.
- Various online resources and academic references on processor pipeline design.
- Consulted LLMs for syntax clarifications on C++ and README.md
