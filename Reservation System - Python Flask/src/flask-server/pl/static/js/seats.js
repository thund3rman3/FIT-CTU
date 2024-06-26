const container = document.querySelector('.container');
const seats = document.querySelectorAll('.row .seat:not(.occupied)');
const count = document.getElementById('count');
const total = document.getElementById('total');
const movieSelect = document.getElementById('movie');


function sendData() {
    arr = []
    var slides = document.getElementsByClassName("seat selected");
    for (var i = 0; i < slides.length; i++) {
        if(slides[i].id){
            arr.push(parseInt(slides[i].id))
        }
    }


    document.getElementById("inp").value = arr;
    document.getElementById('home_form').submit();

}

function setOccupiedSeats(occupiedlist) {
        for (var i = 0; i < occupiedlist.length; i++) {
        st = document.getElementById(occupiedlist[i].toString())
        if (st) {
            st.className += " occupied";
        }
    }

}

//Update total and count
function updateSelectedCount() {
    const selectedSeats = document.querySelectorAll('.row .seat.selected');
    count.innerText = selectedSeats.length;
    if(selectedSeats.length == 0){
        const button = document.getElementById("submitBtn");
        button.disabled = true;
    }else{
        const button = document.getElementById("submitBtn");
        button.disabled = false;
    }
}


//Seat click event
container.addEventListener('click', e => {
    if (e.target.classList.contains('seat') &&
        !e.target.classList.contains('occupied')) {
        e.target.classList.toggle('selected');
    }
    updateSelectedCount();
});
