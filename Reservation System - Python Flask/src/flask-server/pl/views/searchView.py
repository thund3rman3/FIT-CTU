from flask import Blueprint, render_template, request
from dl.db import db
from dl.dao import DAOMovieOperations
from flask_classful import FlaskView, route

dmo = DAOMovieOperations(db.session)


class searchView(FlaskView):
    """Implementation of search
    """

    @route('/')
    def view(self):
        """Shows search

        Returns:
            search.html
        """
        return render_template('search.html')

    @route('/data', methods=['POST'])
    def controller(self):
        """Shows movie data from search

        Returns:
            data.html
        """
        indexForm = request.form.get("input")

        if indexForm:
            found = dmo.find_movie(indexForm, "nazev")
            return render_template('data.html', found=found)

        select = request.form.get('dropdown')
        searchForm = request.form

        found = dmo.find_movie(searchForm['Name'], select)

        return render_template('data.html', found=found)
