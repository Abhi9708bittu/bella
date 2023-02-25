import React from 'react'
import logo from '../../assets/logo.png'
import './NavBar.css'

const NavBar = () => {
  return (
    <div className='header'>
        <img className='logo' src={logo} alt="" />
        <div className="actions">
          <div className="nav">Home</div>
          <div className="nav">Product</div>
          <div className="nav">Faq</div>
          <div className="nav">Contact</div>
        </div>
        </div>
  )
}

export default NavBar