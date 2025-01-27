function updateHourAndMinute() {
    const now = new Date()
    const hours = String(now.getHours()).padStart(2, '0');
    const minutes = String(now.getMinutes()).padStart(2, '0');
    document.getElementById('time-hour').textContent = `${hours}`
    document.getElementById('time-minute').textContent = `${minutes}`
}

function focusButton(index) {
    document.getElementById('buttons').children.item(index).focus()
}


function init() {
    updateHourAndMinute() 
    setInterval(updateHourAndMinute, 1000)
    focusButton(0)
}

init()
