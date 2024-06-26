from datetime import datetime, timedelta
from sqlalchemy.orm import Session
from .daoInterface import DAOAdminOperationsInterface, DAOInterface, DAOMovieOperationsInterface, \
    DAOScreeningOperationsInterface
from .entity import Film, Admin, Promitani, Sal, Uzivatel, Rezervace, Sedadlo, t_Je_na
from sqlalchemy import and_


class DAO(DAOInterface):
    def __init__(self, db: Session):
        self.db = db


class DAOMovieOperations(DAOMovieOperationsInterface):

    def __init__(self, db: Session):
        self.db = db

    def find_movie(self, search_phrase: str, search_by: str) -> Film:

        search_phrase = search_phrase.replace(' ', '%')
        if search_by == "nazev":
            films = self.db.query(Film).filter(Film.nazev.ilike(f'%{search_phrase}%')).all()
        elif search_by == "rok_vydani":
            try:
                films = self.db.query(Film).filter(Film.rok_vydani == (int(search_phrase))).all()
            except:
                return []
        elif search_by == "zanr":
            films = self.db.query(Film).filter(Film.zanr.ilike(f'%{search_phrase}%')).all()
        else:
            return []
        return films

    def get_movie_by_name(self, name: str) -> Film:

        return self.db.query(Film).filter(Film.nazev == name).first()

    def get_movie_by_id(self, id: int) -> Film:
        return self.db.query(Film).get(id)

    def get_movies(self) -> Film:
        return self.db.query(Film).order_by(Film.nazev.asc()).all()

    def add_movie(self, name: str, length: int, year: int, genre: str,
                  about: str) -> bool:
        id = self.db.query(Film).order_by(Film.film_id.desc()).first().film_id + 1
        movie = Film(nazev=name, delka_filmu=length, rok_vydani=year,
                     zanr=genre, popis=about, film_id=id)

        try:
            self.db.add(movie)
            self.db.commit()
        except:
            self.db.rollback()
            return False

        return True


class DAOScreeningOperations(DAOScreeningOperationsInterface):
    def __init__(self, db: Session):
        self.db = db

    def edit_screening(self, screening_id, date, movie_id, room_id) -> bool:
        if not self.check_room_availability(date, movie_id, room_id):
            return False

        screening = self.db.query(Promitani).get(screening_id)

        d = date.split('T')
        screening.datum = d[0]
        screening.cas = d[1]
        screening.film_id = movie_id
        screening.sal_id = room_id

        try:
            self.db.commit()
        except:
            self.db.rollback()
            return False

        return True

    def add_screening(self, date, movie_id, room_id) -> bool:
        if not self.check_room_availability(date, movie_id, room_id):
            return False

        id = self.db.query(Promitani).order_by(Promitani.promitani_id.desc()).first().promitani_id + 1
        d = date.split('T')

        screening = Promitani(promitani_id=id, datum=d[0], cas=d[1],
                              film_id=movie_id, sal_id=room_id)

        try:
            self.db.add(screening)
            self.db.commit()
        except:
            self.db.rollback()
            return False

        return True

    def delete_screening(self, screening_id: int) -> bool:
        try:
            reservations = self.db.query(Rezervace).filter(Rezervace.promitani_id == screening_id).all()
            for reservation in reservations:
                self.db.delete(reservation)
            screening = self.db.query(Promitani).get(screening_id)
            self.db.delete(screening)
            self.db.commit()
        except:
            self.db.rollback()
            return False

        return True

    def get_screening_by_time(self, date) -> (Film, Promitani):
        return self.db.query(Film, Promitani).join(Promitani).filter(Promitani.datum >= date).order_by(Promitani.datum,
                                                                                                       Promitani.cas)

    def get_screening_by_time_until(self, date_start, date_end) -> (Film, Promitani):
        return self.db.query(Film, Promitani).join(Promitani).filter(Promitani.datum >= date_start,
                                                                     Promitani.datum <= date_end).order_by(
            Promitani.datum,
            Promitani.cas)

    def get_reserved_seats(self, screening_id):
        reservatedSeats = self.db.query(Sedadlo).join(t_Je_na).join(Rezervace).filter(
            Rezervace.promitani_id == screening_id).all()
        if reservatedSeats:
            sedadlo = [x.sedadlo_id for x in reservatedSeats]
            return sedadlo
        return []

    def is_seat_reserved(self, seat_id, screening_id) -> bool:
        return self.db.query(Rezervace).join(t_Je_na).join(Sedadlo).filter(Rezervace.promitani_id == screening_id,
                                                                           Sedadlo.sedadlo_id == seat_id).count()

    def add_reservation(self, name, surname, email, screening_id, seatList) -> int:
        user = self.db.query(Uzivatel).filter(Uzivatel.email == email).first()
        newUser = False
        if user is None:
            newUser = True
            userId = self.db.query(Uzivatel).order_by(Uzivatel.uzivatel_id.desc()).first().uzivatel_id + 1
            user = Uzivatel(email=email, jmeno=name, prijmeni=surname,
                            uzivatel_id=userId)
        else:
            userId = user.uzivatel_id

        screeningId = self.db.query(Rezervace).order_by(Rezervace.rezervace_id.desc()).first().rezervace_id + 1
        reservation = Rezervace(cislo_rezervace=screeningId, rezervace_id=screeningId, promitani_id=screening_id,
                                uzivatel_id=userId)

        for seat in seatList:
            if self.is_seat_reserved(seat, screening_id):
                print("sedadlo již zarezervováno")
                return -1

            queryedSeat = self.db.query(Sedadlo).filter(Sedadlo.sedadlo_id == seat).first()
            if queryedSeat:
                reservation.sedadlos.append(queryedSeat)
            else:
                print("sedadlo neexistuje")
                return -1

        try:
            if newUser:
                self.db.add(user)
            self.db.add(reservation)
            self.db.commit()
        except:
            self.db.rollback()
            print("chyba")
            return -1

        return screeningId

    def cancel_reservation(self, reservation_id: int, email: str = None) -> bool:
        try:
            reservation = self.db.query(Rezervace).get(reservation_id)
        except:
            return False

        if email:
            user = self.db.query(Uzivatel).filter(Uzivatel.email == email).first()
            if not user or reservation.uzivatel_id != user.uzivatel_id:
                return False

        try:
            self.db.delete(reservation)
            self.db.commit()
        except:
            return False

        return True

    def get_valid_screenings_by_movie_name(self, date, name) -> (Film, Promitani, Sal):
        return self.db.query(Film, Promitani, Sal).join(Promitani, Promitani.film_id == Film.film_id).join(Sal,
                                                                                                           Sal.sal_id == Promitani.sal_id).filter(
            Promitani.datum >= date).filter(
            Film.nazev == name).order_by(Promitani.datum,
                                         Promitani.cas)

    def get_screening_by_id(self, id: int) -> Promitani:
        return self.db.query(Promitani).get(id)

    def get_seats_by_reservation(self, reservation_id):
        return self.db.query(Sedadlo).join(t_Je_na).join(Rezervace).filter(
            Rezervace.rezervace_id == reservation_id).all()

    def get_rooms(self) -> Sal:
        return self.db.query(Sal).order_by(Sal.cislo.asc()).all()

    def get_room_by_id(self, id: int) -> Sal:
        return self.db.query(Sal).get(id)

    def check_room_availability(self, date, movie_id, room_id) -> bool:
        d = date.split('T')
        starting_at = datetime.strptime(d[1], '%H:%M')

        movie_length = self.db.query(Film).get(movie_id).delka_filmu
        prev_cannot_start_before = starting_at + timedelta(minutes=movie_length)

        room = self.db.query(Promitani).filter(Promitani.sal_id == room_id,
                                               Promitani.datum == d[0],
                                               Promitani.cas < prev_cannot_start_before.time()).all()

        for screening in room:
            movie_length = self.db.query(Film).get(screening.film_id).delka_filmu
            ends_at = screening.cas.hour * 60 + screening.cas.minute + movie_length
            if ends_at > starting_at.hour * 60 + starting_at.minute:
                return False

        return True

    def get_room_by_screening(self, screening_id):
        try:
            return self.db.query(Promitani).filter(screening_id == Promitani.promitani_id).first().sal_id
        except:
            return False


class DAOAdminOperations(DAOAdminOperationsInterface):
    def __init__(self, db: Session):
        self.db = db

    def authenticate(self, email: str, password: str) -> Admin:
        admin = self.db.query(Admin).filter(Admin.email == email).first()

        if admin and admin.heslo == password:
            return admin

        return None

    def get_user_by_id(self, id: int) -> Admin:
        return self.db.query(Admin).get(id)
