// Обновление размера исходного текста в реальном времени
document.getElementById('input').addEventListener('input', function () {
    const text = this.value;
    const sizeInBytes = new Blob([text]).size;
    updateOriginalSize(sizeInBytes);
});

/**
 * Обновляет отображаемый размер исходного текста в статусной строке
 * @param {number} originalSize - Размер в байтах
 */
function updateOriginalSize(originalSize) {
    const statsEl = document.getElementById('stats');
    // Сохраняем текущие значения сжатого размера и процента, если они есть
    const currentText = statsEl.textContent;
    let compressedSize = 0;
    let ratio = '—';

    // Пытаемся извлечь уже вычисленные значения (если были)
    const match = currentText.match(/Сжатый: (\d+) байт/);
    if (match) {
        compressedSize = parseInt(match[1], 10);
        if (originalSize > 0) {
            ratio = ((1 - compressedSize / originalSize) * 100).toFixed(2);
        }
    }

    statsEl.textContent =
        `Исходный размер: ${originalSize} байт | Сжатый: ${compressedSize || '—'} байт | Сжатие: ${ratio}%`;
}

// Функция получения выбранного типа
function getSelectedType() {
    const selected = document.querySelector('input[name="fileType"]:checked');
    return selected ? selected.value : 'html';
}

// Минификаторы
function compressHTML(code) {
    // 1. Временно извлекаем и заменяем <style> блоки
    let styleBlocks = [];
    code = code.replace(/<style\b[^>]*>([\s\S]*?)<\/style>/gi, (match, content) => {
        styleBlocks.push(content);
        return `__STYLE_PLACEHOLDER_${styleBlocks.length - 1}__`;
    });

    // 2. Извлекаем и заменяем внутренние <script> блоки (без src)
    let scriptBlocks = [];
    code = code.replace(/<script\b(?![^>]*\bsrc=)([^>]*)>([\s\S]*?)<\/script>/gi, (match, attrs, content) => {
        scriptBlocks.push(content);
        return `__SCRIPT_PLACEHOLDER_${scriptBlocks.length - 1}__`;
    });

    // 3. Удаляем HTML-комментарии
    code = code.replace(/<!--[\s\S]*?-->/g, '');

    // 4. Сжимаем HTML-разметку
    code = code
        .replace(/\s+/g, ' ')
        .replace(/\s*([<>])\s*/g, '$1')
        .trim();

    // 5. Сжимаем извлечённые CSS-блоки и возвращаем их
    styleBlocks = styleBlocks.map(css => compressCSS(css));
    code = code.replace(/__STYLE_PLACEHOLDER_(\d+)__/g, (match, index) => {
        return `<style>${styleBlocks[index]}</style>`;
    });

    // 6. Сжимаем извлечённые JS-блоки и возвращаем их
    scriptBlocks = scriptBlocks.map(js => compressJS(js));
    code = code.replace(/__SCRIPT_PLACEHOLDER_(\d+)__/g, (match, index) => {
        return `<script>${scriptBlocks[index]}</script>`;
    });

    return code;
}

function compressCSS(code) {
    return code
        .replace(/\/\*[\s\S]*?\*\//g, '')
        .replace(/\s*([{}:;,>+~=\(\)\[\]])\s*/g, '$1')
        .replace(/\s+/g, ' ')
        .replace(/\{\s+/g, '{')
        .replace(/\s+\}/g, '}')
        .replace(/;\s*}/g, '}')
        .replace(/\s*!\s*important/g, '!important')
        .trim();
}

function compressJS(code) {
    return code
        .replace(/\/\/.*$/gm, '')
        .replace(/\/\*[\s\S]*?\*\//g, '')
        .replace(/\s+/g, ' ')
        .replace(/\s*([{}()\[\];,=:+\-*/&|!?<>.%])\s*/g, '$1')
        .trim();
}

// Основная функция сжатия
function compress() {
    const inputEl = document.getElementById('input');
    const outputEl = document.getElementById('output');
    const input = inputEl.value;

    if (!input.trim()) {
        alert('Пожалуйста, вставьте код для сжатия.');
        return;
    }

    const type = getSelectedType();
    let compressed = '';

    if (type === 'html') {
        compressed = compressHTML(input);
    } else if (type === 'css') {
        compressed = compressCSS(input);
    } else if (type === 'js') {
        compressed = compressJS(input);
    }

    outputEl.value = compressed;

    // Обновляем статистику полностью
    const originalSize = new Blob([input]).size;
    const compressedSize = new Blob([compressed]).size;
    const ratio = originalSize > 0 ? ((1 - compressedSize / originalSize) * 100).toFixed(2) : '0.00';

    document.getElementById('stats').textContent =
        `Исходный размер: ${originalSize} байт | Сжатый: ${compressedSize} байт | Сжатие: ${ratio}%`;
}

// Функция очистки
function clearInput() {
    document.getElementById('input').value = '';
    document.getElementById('output').value = '';
    document.getElementById('stats').textContent = 'Исходный размер: 0 байт | Сжатый: — байт | Сжатие: —%';
}

// Функция копирования результата
function copyResult() {
    const output = document.getElementById('output');
    if (!output.value.trim()) {
        alert('Нет результата для копирования.');
        return;
    }
    output.select();
    output.setSelectionRange(0, 99999); // Для мобильных
    document.execCommand('copy');
    // Можно добавить всплывающую подсказку "Скопировано!" позже
}