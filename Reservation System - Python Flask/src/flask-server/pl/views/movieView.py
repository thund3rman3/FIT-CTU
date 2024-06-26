from flask import Blueprint, render_template, request
from dl.db import db
from dl.dao import DAOMovieOperations, DAOScreeningOperations
import datetime
from flask_classful import FlaskView, route

dmo = DAOMovieOperations(db.session)
dso = DAOScreeningOperations(db.session)


class movieView(FlaskView):
    """Implementation of view for movie
    """

    @route('/<movie_id>', methods=['GET'])
    def movie_view(self, movie_id):
        """Shows movie

        Args:
            movie_id: id of movie

        Returns:
            movie.html
        """
        film = dmo.get_movie_by_id(movie_id)
        screenings = dso.get_valid_screenings_by_movie_name(datetime.date.today(), film.nazev)
        screenings = [(x, len(dso.get_reserved_seats(x[1].promitani_id))) for x in screenings]

        return render_template('movie.html', film=film, screenings=screenings)
