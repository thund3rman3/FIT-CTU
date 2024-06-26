from flask import redirect, render_template, request
from flask_login import login_required, login_user
from dl.db import db
from dl.dao import DAOAdminOperations, DAOMovieOperations, DAOScreeningOperations
import hashlib
import datetime
from flask_classful import FlaskView, route

dao = DAOAdminOperations(db.session)
dmo = DAOMovieOperations(db.session)
dso = DAOScreeningOperations(db.session)


class adminView(FlaskView):
    """Implementation of admin related methods.
    """

    @route('/', methods=['GET'])
    def login_view(self):
        """Loging page for admin.

         Returns:
            admin_login.html
         """
        return render_template('admin_login.html', failed_login=False)

    @route('/', methods=['POST'])
    def login_controller(self):
        """Loging page for admin.

         Returns:
             if admin is logged: admin_dashboard.html
             else: admin_login.html
         """

        email = request.form['Email']
        password = hashlib.sha512(str(request.form['Password']).encode('utf-8')).hexdigest()

        admin = dao.authenticate(email, password)

        if admin:
            login_user(admin)
            return redirect('dashboard')

        return render_template('admin_login.html', failed_login=True)

    @route('/dashboard', methods=['GET'])
    @login_required
    def dashboard_view(self):
        """Shows dashboard.

        Returns:
            admin_dashboard.html
        """
        return render_template('admin_dashboard.html', add=False, edit=False, msg='')

    @route('/dashboard/add', methods=['GET'])
    @login_required
    def add_movie_view(self):
        """Adds movie.

        Returns:
            admin_dashboard.html
        """
        return render_template('admin_dashboard.html', add=True, edit=False, msg='')

    @route('/dashboard/add', methods=['POST'])
    @login_required
    def add_movie_controller(self):
        """Adds movie.

        Returns:
            admin_dashboard.html
        """

        movie = request.form
        added = dmo.add_movie(movie['Name'], movie['Length'], movie['Year'],
                              movie['Genre'], movie['About'])

        if added:
            msg = 'Film byl úspěšně přidán.'
        else:
            msg = 'Při přidávání filmu se vyskytla chyba.'

        return render_template('admin_dashboard.html', add=True, edit=False, msg=msg)

    @route('/dashboard/edit', methods=['GET'])
    @login_required
    def edit_view(self):
        """Shows screening by time.

        Returns:
            admin_dashboard.html
        """

        edit = dso.get_screening_by_time(datetime.date.today())
        return render_template('admin_dashboard.html', add=False, edit=edit, msg='')

    @route('/dashboard/edit/<screening_id>', methods=['GET']
           )
    @login_required
    def edit_specific_view(self, screening_id):
        """Shows screening by screening id.

        Args:
            screening_id: id of screening
        Returns:
            admin_schedule.html
        """
        screening = dso.get_screening_by_id(screening_id)

        date = f'{screening.datum}T{screening.cas}'

        return render_template('admin_schedule.html', edit=True,
                               date=date,
                               film=screening.film,
                               sal=screening.sal,
                               movies=dmo.get_movies(),
                               rooms=dso.get_rooms())

    @route('/dashboard/edit/<screening_id>', methods=['POST'])
    @login_required
    def edit_screening_controller(self, screening_id):
        """Edits screening.
        Args:
            screening_id: id of screening
        Returns:
            if screening was edited: admin_dashboard.html
            if screening was not edited: admin_schedule.html
        """

        movie_id = request.form['Movie']
        room_id = request.form['Room']
        date = request.form['Date']

        if request.form['Action'] == 'Odstranit':
            changed = dso.delete_screening(screening_id)

        else:
            changed = dso.edit_screening(screening_id, date, movie_id, room_id)

        if changed:
            msg = 'Harmonogram upraven.'
            return render_template('admin_dashboard.html', add=False, edit=False,
                                   msg=msg)

        msg = 'Při editaci harmonogramu nastala chyba.'
        return render_template('admin_schedule.html', edit=True,
                               date=date,
                               film=dmo.get_movie_by_id(movie_id),
                               sal=dso.get_room_by_id(room_id),
                               movies=dmo.get_movies(),
                               rooms=dso.get_rooms(),
                               msg=msg)

    @route('/dashboard/edit/new', methods=['GET'])
    @login_required
    def add_screening_view(self):
        """Adds screening.

        Returns:
            admin_schedule.html
        """
        return render_template('admin_schedule.html', edit=False,
                               movies=dmo.get_movies(),
                               rooms=dso.get_rooms(),
                               msg='')

    @route('/dashboard/edit/new', methods=['POST'])
    @login_required
    def add_screening_controller(self):
        """Adds screening.

        Returns:
            if the reservation was added: admin_dashboard.html
            if the reservation was not added: admin_schedule.html
        """
        movie_id = request.form['Movie']
        room_id = request.form['Room']
        date = request.form['Date']

        added = dso.add_screening(date, movie_id, room_id)

        if added:
            msg = 'Promítání úspěšně přidáno.'
            return render_template('admin_dashboard.html', add=False, edit=False,
                                   msg=msg)

        msg = 'Při přidávání promítání nastala chyba.'
        return render_template('admin_schedule.html', edit=True,
                               date=date,
                               film=dmo.get_movie_by_id(movie_id),
                               sal=dso.get_room_by_id(room_id),
                               movies=dmo.get_movies(),
                               rooms=dso.get_rooms(),
                               msg=msg)

    @route('/dashboard/reservation', methods=['GET'])
    @login_required
    def cancel_reservation_view(self):
        """Cancels reservation.

        Returns:
            reservation_cancel.html
        """
        return render_template('reservation_cancel.html', admin=True)

    @route('/dashboard/reservation', methods=['POST'])
    @login_required
    def cancel_reservation_controller(self):
        """Checks if the reservation is canceled.

        Returns:
            if the reservation was not canceled: admin_dashboard.html
            if the reservation was canceled: reservation_cancel.html
        """
        reservation_id = request.form['Number']

        canceled = dso.cancel_reservation_view(reservation_id, None)

        if canceled:
            msg = 'Zrušení rezervace úspěšné.'
            return render_template('admin_dashboard.html', add=False, edit=False, msg=msg)

        msg = 'Zrušení rezervace se nezdařilo.'
        return render_template('reservation_cancel.html', admin=True)
