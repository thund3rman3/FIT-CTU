from flask import Blueprint, render_template, request
from dl.db import db
from dl.dao import DAOMovieOperations, DAOScreeningOperations
from .mailSender import mail_sender
from flask import session
from flask_classful import FlaskView, route

dmo = DAOMovieOperations(db.session)
dso = DAOScreeningOperations(db.session)


class reservationView(FlaskView):
    """Implementation of reservation
    """

    @route('/', methods=['GET'])
    def cancel_view(self):
        """Cancels reservation

          Returns:
             reservation_cancel.html
        """
        return render_template('reservation_cancel.html', admin=False)

    @route('/', methods=['POST'])
    def cancel_reservation_controller(self):
        """Cancels reservation

          Returns:
            if reservation was canceled properly:   reservation_confirmation.html
            if reservation was not canceled properly:   reservation_cancel.html
        """

        reservation_id = request.form['Number']
        email = request.form['Email']

        canceled = dso.cancel_reservation_view(reservation_id, email)

        if canceled:
            return render_template('reservation_confirmation.html',
                                   reservation_number=0, cancel=True)

        msg = 'Zrušení rezervace se nezdařilo.'
        return render_template('reservation_cancel.html', admin=True, msg=msg)

    @route('/<screening>/reservation', methods=['GET'])
    def reservation_screening_view(self, screening):
        """Shows available seats

          Returns:
            reservation_seats.html
        """

        occupiedSeats = dso.get_reserved_seats(screening)
        print("occupiedSeats: ", occupiedSeats)

        return render_template('reservation_seats.html', occupiedSeats=occupiedSeats, screening_id=screening)

    @route('/<screening>/confirmation', methods=['POST'])
    def reservation_confirmation_controller(self, screening):
        """Confirms reservation.

        Args:
            screening: screening table
        Returns:
            reservation_seats.html
        """

        if request.method == "POST":
            name = request.form.get('name')
            surname = request.form.get('surname')
            email = request.form.get('email')
            emailConfirmation = request.form.get('emailConfirmation')

            if emailConfirmation != email:
                return render_template("error_template.html", number=1)

            selectedSeats = session.get("seats", None)
            session.clear()

            reservation_number = dso.add_reservation(name, surname, email, screening, selectedSeats)
            if reservation_number > 0 and selectedSeats:
                seats = dso.get_seats_by_reservation(reservation_number)
                mail_sender().send_mail(email, reservation_number, dso.get_room_by_screening(screening),
                                        self.seats_to_str(seats))

            return render_template("reservation_confirmation.html",
                                   reservation_number=reservation_number,
                                   cancel=False)

    @route('/<screening>/reservation_form', methods=['POST'])
    def reservation_form_controller(self, screening):
        """Confirms reservation.

        Args:
            screening: row from Promitani table
        Returns:
            reservation_form.html
        """

        if request.method == "POST":
            seats = request.form.get('inp')
            if seats:
                try:
                    session["seats"] = [int(x) for x in seats.split(",")]
                except:
                    return "ERROR"

            return render_template("reservation_form.html", screening_id=screening)

    def seats_to_str(self, seats):
        """Confirms reservation.

        Args:
            seats: rows from Sedadlo table
        Returns:
            seatStr: string, including infomation about row and seat number of picked seats
        """

        seatStr = f'řada: {seats[0].rada} sedadlo: {seats[0].cislo} '
        for idx in range(1, len(seats)):
            if seats[idx].rada == seats[idx - 1].rada:
                seatStr += f'{seats[idx].cislo} '
            else:
                seatStr += f'<br>řada: {seats[idx].rada} sedadlo: {seats[idx].cislo} '

        return seatStr
