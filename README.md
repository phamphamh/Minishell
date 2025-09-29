# Minishell

## 📖 Description
**Minishell** is a project that aims to recreate a minimal Unix shell.  
It executes commands, manages processes, and implements a subset of **bash** features.  
The goal is to understand the internal mechanisms of a command-line interpreter and practice low-level **Unix/Linux** concepts such as **processes**, **pipes**, and **redirections**.

---

## 🚀 Features
- Execution of simple commands with arguments.  
- Path resolution using the `PATH` environment variable.  
- Implementation of the following **builtins**:
  - `echo` (with `-n` option)  
  - `cd`  
  - `pwd`  
  - `export`  
  - `unset`  
  - `env`  
  - `exit`  
- Redirections:  
  - `<` (input from file)  
  - `>` (output to file, overwrite)  
  - `>>` (output to file, append)  
  - `<<` (heredoc)  
- Pipes (`|`) between commands.  
- Signal handling (`Ctrl-C`, `Ctrl-D`, `Ctrl-\`).  
- Environment variables management.  
- Behavior closely matching `bash` for the specified cases.

---

## ⚙️ Installation & Compilation
Clone the repository and compile with **make**:

```bash
git clone https://github.com/your-username/minishell.git
cd minishell
make
````

This will generate the **`minishell`** executable.

---

## ▶️ Usage

Run the shell:

```bash
./minishell
```

Example session:

```bash
minishell$ echo "Hello World"
Hello World

minishell$ ls -l | grep minishell
```

---

## 📂 Project Structure

```
.
├── Makefile        # Compilation rules
├── minishell.h     # Main header file
├── src/            # Source code
│   ├── main.c
│   ├── parser.c
│   ├── executor.c
│   ├── builtins/
│   └── ...
├── libft/          # Utility library
└── README.md
```

---

## 🛠️ Technologies

* Language: **C**
* Coding style: **Norminette** (42 school standard)
* System calls: `fork`, `execve`, `pipe`, `dup2`, `signal`, `waitpid`, etc.

---

## 👥 Authors

* [Your Name]
* [Your partner’s name, if applicable]

---

## ✅ Learning Goals

* Understand how a shell works internally.
* Practice **system programming** in C.
* Improve memory management and pointer handling.
* Work with **processes** and **inter-process communication**.
