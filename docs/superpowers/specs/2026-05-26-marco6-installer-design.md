# Marco 6 — Execução de Comandos com Saída em Tempo Real

**Data:** 2026-05-26

---

## Objetivo

Implementar a instalação de pacotes via `-i` (resumo final) e `-v` (saída em tempo real), usando `popen` com redirecionamento `2>&1`.

---

## Comportamento

| Flag | Comportamento |
|------|--------------|
| `-i` | Instala todos os pacotes silenciosamente, imprime apenas `"Download dos pacotes foram concluídos com X falhas e Y sucessos"` ao final |
| `-v` | Instala todos os pacotes imprimindo cada linha de saída do comando em tempo real |

---

## Arquitetura

### 1. `package_manager.c` — nova função pública

```c
int pm_get_installer_cmds(char names[][100], char cmds[][255], int max);
```

- Lê `packages.json` via `load_file()`
- Preenche `names[]` e `cmds[]` com os campos `"name"` e `"installer-cmd"` de cada pacote
- Retorna a quantidade de pacotes encontrados (máx: `max`)

### 2. `installer.c` — duas funções

```c
// Retorna 0 em sucesso, -1 em falha
int install_package(const char *name, const char *cmd, int verbose);

// Instala todos os pacotes; retorna 0
int run_installer(int verbose);
```

**`install_package`:**
- Constrói `"<cmd> 2>&1"` com `snprintf`
- Abre com `popen(full_cmd, "r")`
- Lê linha a linha com `fgets`
  - Se `verbose == 1`: imprime cada linha com `printf`
  - Se `verbose == 0`: descarta a linha
- Fecha com `pclose()` e verifica exit code
- Retorna `0` (sucesso) ou `-1` (falha)

**`run_installer`:**
- Chama `pm_get_installer_cmds()` (máx 50 pacotes)
- Itera os pacotes, chamando `install_package` para cada um
- Contabiliza `successes` e `failures`
- Se `verbose == 0`: imprime resumo final
- Se `verbose == 1`: imprime cabeçalho por pacote (`"==> Instalando: <nome>"`)

### 3. `cli.c` — correção de bug e conexão

- Corrige opção `--install-packages-verbose`: troca char `'iv'` por `'v'`
- Atualiza string de `getopt_long` de `"hlaudiiv"` para `"hlaudiv"`
- Conecta `case 'i'` → `run_installer(0)`
- Conecta `case 'v'` → `run_installer(1)`

---

## Limites

- Máximo de 50 pacotes (array estático)
- Nomes: 100 chars | Comandos: 255 chars
- Sem tratamento de `sudo` interativo — usuário deve ter permissão prévia

---

## Arquivos alterados

- `src/installer.c` — implementação completa
- `src/package_manager.c` — adicionar `pm_get_installer_cmds`
- `include/package_manager.h` — declarar `pm_get_installer_cmds`
- `include/installer.h` — declarar `install_package`
- `src/cli.c` — corrigir bug e conectar flags
