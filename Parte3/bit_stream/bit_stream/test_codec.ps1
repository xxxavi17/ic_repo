# Script de Exemplo - DCT Codec
# PowerShell script para testar o codec

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   DCT Audio Codec - Script de Teste" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Verificar se os executáveis existem
$binDir = "..\bin"
$encoder = "$binDir\dct_encoder.exe"
$decoder = "$binDir\dct_decoder.exe"
$test = "$binDir\dct_test.exe"

if (-not (Test-Path $encoder)) {
    Write-Host "ERRO: Executáveis não encontrados!" -ForegroundColor Red
    Write-Host "Por favor, compile primeiro:" -ForegroundColor Yellow
    Write-Host "  cd src" -ForegroundColor Yellow
    Write-Host "  make" -ForegroundColor Yellow
    exit 1
}

# Exemplo 1: Codificar e descodificar um ficheiro
Write-Host "=== Exemplo 1: Codificar e Descodificar ===" -ForegroundColor Green
Write-Host ""

$inputFile = "..\..\..\audio_files\sample01.wav"

if (Test-Path $inputFile) {
    Write-Host "Codificando $inputFile..." -ForegroundColor Yellow
    & $encoder $inputFile "sample01_compressed.dct" 512 256 2
    
    Write-Host ""
    Write-Host "Descodificando sample01_compressed.dct..." -ForegroundColor Yellow
    & $decoder "sample01_compressed.dct" "sample01_decoded.wav"
    
    Write-Host ""
    Write-Host "Ficheiros criados:" -ForegroundColor Cyan
    Write-Host "  - sample01_compressed.dct (ficheiro comprimido)" -ForegroundColor White
    Write-Host "  - sample01_decoded.wav (áudio reconstruído)" -ForegroundColor White
} else {
    Write-Host "AVISO: Ficheiro $inputFile não encontrado" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "=== Exemplo 2: Testar com Vários Parâmetros ===" -ForegroundColor Green
Write-Host ""
Write-Host "Executando testes com 7 configurações diferentes..." -ForegroundColor Yellow
Write-Host ""

if (Test-Path $inputFile) {
    & $test $inputFile
    
    Write-Host ""
    Write-Host "Relatório gerado: dct_codec_results.csv" -ForegroundColor Cyan
    Write-Host "Pode abrir no Excel ou outro programa de folhas de cálculo" -ForegroundColor White
} else {
    Write-Host "Para executar testes em todos os ficheiros:" -ForegroundColor Yellow
    Write-Host "  cd ..\bin" -ForegroundColor White
    Write-Host "  .\dct_test.exe" -ForegroundColor White
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   Exemplos concluídos!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
