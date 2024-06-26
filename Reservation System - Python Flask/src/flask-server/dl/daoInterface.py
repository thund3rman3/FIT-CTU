from abc import ABC, abstractmethod
from sqlalchemy.orm import Session
from .entity import Film, Admin, Promitani, Sal, Sedadlo, Rezervace


class DAOInterface(ABC):
    """Interface for all classes.
    """

    @abstractmethod
    def __init__(self, db: Session):
        """Initializer common for CRUD classes,
        needs session on which it will carry on operations.

        Args:
            db (Session): Session used to query database
        """

        pass


class DAOMovieOperationsInterface(DAOInterface):
    """Interface for movie methods.
    """

    @abstractmethod
    def find_movie(self, search_phrase: str, search_by: str) -> Film:
        """Find movies with similiar names as a phrase.

        Args:
            search_phrase: wanted movie name
            search_by: <nazev | rok_vydani | zanr>

        Returns:
            Film table with results similiar to search_phrase
        """
        pass

    @abstractmethod
    def get_movie_by_name(self, name: str) -> Film:
        """Get movie from database based on name.

        Args:
            name: name of movie

        Returns:
            Film object of movie with given name
        """
        pass

    @abstractmethod
    def get_movie_by_id(self, id: int) -> Film:
        """Get movie from database based on its id.

        Args:
            id: id of movie

        Returns:
            Film object of movie with given id
        """
        pass

    @abstractmethod
    def get_movies(self) -> Film:
        """Return all movies in database.
        """
        pass

    @abstractmethod
    def add_movie(self, name: str, length: int, year: int, genre: str,
                  about: str) -> bool:
        """Add movie with specific attributes to database.
        
        Args:
            name: movie name
            length: duration in minutes
            year: year of release
            genre: genre of movie
            about: short description of movie
            
        Returns:
            Boolean indicating if adding movie succeeded
        """
        pass


class DAOScreeningOperationsInterface(DAOInterface):
    """Interface for screening methods.
    """

    @abstractmethod
    def edit_screening(self, screening_id, date, movie_id, room_id) -> bool:
        """Change schedule for a screening.
        
        Args:
            screening_id: id of screening
            date: new date to set
            movie_id: id of new movie to set
            room_id: id of new room to set
            
        Returns:
            Boolean indicating if operation succeded
        """
        pass

    @abstractmethod
    def add_screening(self, date, movie_id, room_id) -> bool:
        """Add a new screening.
        
        Args:
            date: date of screening
            movie_id: id of shown movie
            room_id: id of room
            
        Returns:
            Boolean indicating if operation succeded
        """
        pass

    @abstractmethod
    def delete_screening(self, screening_id: int) -> bool:
        """Delete a screening.
        
        Args:
            screening_id: id of screening to delete
            
        Returns:
            Boolean indicating if operation succeded
        """
        pass

    @abstractmethod
    def get_screening_by_time(self, date) -> (Film, Promitani):
        """Get Film joined with Promitani from database played since date.

        Args:
            date: date since when films are played

        Returns:
            list of (Film, Promitani) object of movie with given name ordered by date and time
        """
        pass

    @abstractmethod
    def get_screening_by_time_until(self, date_start, date_end) -> (Film, Promitani):
        """Get Film joined with Promitani from database played since date_start to date_end.

        Args:
            date_start: date since when films are played
            date_end: date to films are played

        Returns:
            list of (Film, Promitani) object of movie with given name ordered by date and time
        """
        pass

    @abstractmethod
    def get_screening_by_id(self, id: int) -> Promitani:
        """Get screening object from database based on its id.
        
        Args:
            id: id of screening
            
        Returns:
            Screening object with given id
        """
        pass

    @abstractmethod
    def get_reserved_seats(self, screening_id):
        """Get all seats that are reserved for the screening with screening_id.

        Args:
            screening_id: id of screening

        Returns:
            Reserved seat objects
        """
        pass

    @abstractmethod
    def is_seat_reserved(self, seat_id, screening_id) -> bool:
        """Check if seat with seat_id is reserved for the screening with screening_id

           Args:
               screening_id: id of screening
               seat_id: id of seat

           Returns:
               True if seat is reserved for the screening
           """
        pass

    @abstractmethod
    def add_reservation(self, name, surname, email, screening_id, seatList) -> int:
        """Add User and Reservation to database

            Args:
                name: user's firstname
                surname: user's surname
                email: user's email
                screening_id: id of screening
                seatList: list of seatObjects

            Returns:
                Number of reservation if reservation was added else -1
            """
        pass

    @abstractmethod
    def cancel_reservation(self, reservation_id: int, email: str = None) -> bool:
        """Cancels Reservation

             Args:
                 reservation_id: id of reservation
                 email: user's email

             Returns:
                 True if reservation was canceled else False
             """
        pass

    @abstractmethod
    def get_valid_screenings_by_movie_name(self, date, name) -> (Film, Promitani, Sal):
        """Get all screenings of movie since date

             Args:
                 date: date since when screenings are displayed
                 name: movie name

             Returns:
                 List of All [(Film, Promitani, Sal)] with name since date
             """
        pass

    @abstractmethod
    def get_seats_by_reservation(self, reservation_id):
        """Get all seats with given reservation

             Args:
                 reservation_id: id of reservation

             Returns:
                 All seats reserved with reservation_id
             """
        pass

    @abstractmethod
    def get_rooms(self) -> Sal:
        """Get all rooms in database.
        Returns:
            Room objects
        """
        pass

    @abstractmethod
    def get_room_by_id(self, id: int) -> Sal:
        """Get room with given id.
        
        Args:
            id: id of wanted room
            
        Returns:
            Room object with given id
        """
        pass

    @abstractmethod
    def get_room_by_screening(self, screening_id) -> Sal:
        """Get room where the screening is occured.

           Args:
               screening_id: id of screening

           Returns:
               Room object with given id
           """
        pass

    @abstractmethod
    def check_room_availability(self, date, movie_id, room_id) -> bool:
        """Check if screening room is still available.

           Args:
               date: date
               movie_id: id of movie
               room_id: id of room

           Returns:
               True if room is available for screening
           """
        pass


class DAOAdminOperationsInterface(DAOInterface):
    """Interface for admin methods.
    """

    @abstractmethod
    def authenticate(self, email: str, password: str) -> Admin:
        """Check if admin credentials match database entry.
        
        Args:
            email: admin email
            password: hashed admin password
            
        Returns:
            Admin object if credentials match, None else
        """
        pass

    @abstractmethod
    def get_user_by_id(self, id):
        """Get user by his id.

        Args:
            id: id of wanted user

        Returns:
            User object with given id
        """

        pass
