const canvas = <HTMLCanvasElement>(document.getElementById('ctx'));

/*
** Disable Context Menu
*/
function disableContextMenu() {
    document.addEventListener('contextmenu', (e) => {
        e.preventDefault();
        return false;
    });
    canvas.addEventListener('contextmenu', (e) => {
        e.preventDefault();
        return false;
    });
}
disableContextMenu();

/*
** Resize Window
*/
function resize(): void {
    const width = window.innerWidth * window.devicePixelRatio;
    const height = window.innerHeight * window.devicePixelRatio;
    canvas.width = Math.max(1, width);
    canvas.height = Math.max(1, height);

    canvas.style.width = window.innerWidth + 'px';
    canvas.style.height = window.innerHeight + 'px';
}
resize();
window.addEventListener('resize', resize);