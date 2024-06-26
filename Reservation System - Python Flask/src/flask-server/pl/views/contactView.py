from flask import Blueprint, render_template, request
from dl.db import Database
from dl.dao import DAOMovieOperations
from flask_classful import FlaskView, route

db = Database()
dmo = DAOMovieOperations(db.session)


class contactView(FlaskView):
    """Implementation of contact related methods.
    """

    @route('/', methods=['GET'])
    def contact_view(self):
        """Shows contact page

         Returns:
            contact.html
         """
        return render_template('contact.html')
