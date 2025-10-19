Write-Host "========================================" -ForegroundColor Cyan# Script de Compilação Simples - Sem CMake

Write-Host "   Compilacao Direta (Sem CMake)" -ForegroundColor Cyan# Para usar quando CMake tiver problemas

Write-Host "========================================" -ForegroundColor Cyan

Write-Host ""Write-Host "========================================" -ForegroundColor Cyan

Write-Host "   Compilação Direta (Sem CMake)" -ForegroundColor Cyan

$gxx = Get-Command g++ -ErrorAction SilentlyContinueWrite-Host "========================================" -ForegroundColor Cyan

if (-not $gxx) {Write-Host ""

    Write-Host "ERRO: g++ nao encontrado!" -ForegroundColor Red

    exit 1# Verificar se g++ está disponível

}$gxx = Get-Command g++ -ErrorAction SilentlyContinue

if (-not $gxx) {

Write-Host "Compilador encontrado: $($gxx.Source)" -ForegroundColor Green    Write-Host "ERRO: g++ não encontrado!" -ForegroundColor Red

Write-Host ""    Write-Host ""

    Write-Host "Por favor, instale MinGW-w64:" -ForegroundColor Yellow

Set-Location src    Write-Host "  1. Download: https://www.mingw-w64.org/downloads/" -ForegroundColor Cyan

    Write-Host "  2. Instalar e adicionar ao PATH" -ForegroundColor White

$binDir = "..\bin"    Write-Host ""

if (-not (Test-Path $binDir)) {    Write-Host "OU instale via MSYS2:" -ForegroundColor Yellow

    New-Item -ItemType Directory -Path $binDir | Out-Null    Write-Host "  1. Download: https://www.msys2.org/" -ForegroundColor Cyan

    Write-Host "Diretorio bin/ criado" -ForegroundColor Green    Write-Host "  2. Execute: pacman -S mingw-w64-x86_64-gcc" -ForegroundColor White

}    Write-Host "  3. Adicione C:\msys64\mingw64\bin ao PATH" -ForegroundColor White

    Write-Host ""

$flags = "-Wall -Wextra -std=c++20 -O3"    exit 1

}

Write-Host "Compilando ficheiros comuns..." -ForegroundColor Yellow

Write-Host "Compilador encontrado: $($gxx.Source)" -ForegroundColor Green

g++ $flags -c bit_stream.cpp -o bit_stream.oWrite-Host ""

if ($LASTEXITCODE -ne 0) { Set-Location ..; exit 1 }

# Ir para o diretório src

g++ $flags -c byte_stream.cpp -o byte_stream.oSet-Location src

if ($LASTEXITCODE -ne 0) { Set-Location ..; exit 1 }

# Criar diretório bin

Write-Host "Compilando codec DCT..." -ForegroundColor Yellow$binDir = "..\bin"

if (-not (Test-Path $binDir)) {

g++ $flags -c dct_codec.cpp -o dct_codec.o    New-Item -ItemType Directory -Path $binDir | Out-Null

if ($LASTEXITCODE -ne 0) { Set-Location ..; exit 1 }    Write-Host "Diretório bin/ criado" -ForegroundColor Green

}

Write-Host "Compilando encoder..." -ForegroundColor Yellow

# Flags de compilação

g++ $flags -o "$binDir\dct_encoder.exe" dct_encoder.cpp bit_stream.o byte_stream.o dct_codec.o$flags = "-Wall -Wextra -std=c++20 -O3"

if ($LASTEXITCODE -ne 0) { 

    Write-Host "ERRO: Falha ao compilar encoder" -ForegroundColor RedWrite-Host "Compilando ficheiros comuns..." -ForegroundColor Yellow

    Set-Location ..

    exit 1# Compilar bit_stream e byte_stream

}g++ $flags -c bit_stream.cpp -o bit_stream.o

Write-Host "  OK dct_encoder.exe" -ForegroundColor Greenif ($LASTEXITCODE -ne 0) { Set-Location ..; exit 1 }



Write-Host "Compilando decoder..." -ForegroundColor Yellowg++ $flags -c byte_stream.cpp -o byte_stream.o

if ($LASTEXITCODE -ne 0) { Set-Location ..; exit 1 }

g++ $flags -o "$binDir\dct_decoder.exe" dct_decoder.cpp bit_stream.o byte_stream.o dct_codec.o

if ($LASTEXITCODE -ne 0) { Write-Host "Compilando codec DCT..." -ForegroundColor Yellow

    Write-Host "ERRO: Falha ao compilar decoder" -ForegroundColor Red

    Set-Location ..# Compilar dct_codec

    exit 1g++ $flags -c dct_codec.cpp -o dct_codec.o

}if ($LASTEXITCODE -ne 0) { Set-Location ..; exit 1 }

Write-Host "  OK dct_decoder.exe" -ForegroundColor Green

Write-Host "Compilando encoder..." -ForegroundColor Yellow

Write-Host "Compilando programa de testes..." -ForegroundColor Yellow

# Compilar encoder

g++ $flags -o "$binDir\dct_test.exe" dct_test.cpp bit_stream.o byte_stream.o dct_codec.og++ $flags -o "$binDir\dct_encoder.exe" dct_encoder.cpp bit_stream.o byte_stream.o dct_codec.o

if ($LASTEXITCODE -ne 0) { if ($LASTEXITCODE -ne 0) { 

    Write-Host "ERRO: Falha ao compilar test" -ForegroundColor Red    Write-Host "ERRO: Falha ao compilar encoder" -ForegroundColor Red

    Set-Location ..    Set-Location ..

    exit 1    exit 1

}}

Write-Host "  OK dct_test.exe" -ForegroundColor GreenWrite-Host "  ✓ dct_encoder.exe" -ForegroundColor Green



Write-Host ""Write-Host "Compilando decoder..." -ForegroundColor Yellow

Write-Host "Limpando ficheiros temporarios..." -ForegroundColor Yellow

Remove-Item *.o -ErrorAction SilentlyContinue# Compilar decoder

g++ $flags -o "$binDir\dct_decoder.exe" dct_decoder.cpp bit_stream.o byte_stream.o dct_codec.o

Set-Location ..if ($LASTEXITCODE -ne 0) { 

    Write-Host "ERRO: Falha ao compilar decoder" -ForegroundColor Red

Write-Host ""    Set-Location ..

Write-Host "========================================" -ForegroundColor Cyan    exit 1

Write-Host "   Compilacao concluida com sucesso!" -ForegroundColor Green}

Write-Host "========================================" -ForegroundColor CyanWrite-Host "  ✓ dct_decoder.exe" -ForegroundColor Green

Write-Host ""

Write-Host "Executaveis criados em: bin" -ForegroundColor WhiteWrite-Host "Compilando programa de testes..." -ForegroundColor Yellow

Write-Host ""

# Compilar test
g++ $flags -o "$binDir\dct_test.exe" dct_test.cpp bit_stream.o byte_stream.o dct_codec.o
if ($LASTEXITCODE -ne 0) { 
    Write-Host "ERRO: Falha ao compilar test" -ForegroundColor Red
    Set-Location ..
    exit 1
}
Write-Host "  ✓ dct_test.exe" -ForegroundColor Green

# Limpar ficheiros objeto
Write-Host ""
Write-Host "Limpando ficheiros temporários..." -ForegroundColor Yellow
Remove-Item *.o -ErrorAction SilentlyContinue

Set-Location ..

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   Compilação concluída com sucesso!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Executáveis criados em: bin\" -ForegroundColor White
Write-Host "  - dct_encoder.exe" -ForegroundColor White
Write-Host "  - dct_decoder.exe" -ForegroundColor White
Write-Host "  - dct_test.exe" -ForegroundColor White
Write-Host ""
Write-Host "Para testar:" -ForegroundColor Yellow
Write-Host "  cd bin" -ForegroundColor White
Write-Host "  .\dct_test.exe" -ForegroundColor White
Write-Host ""
