import React from 'react'
import './Button.css'

const Button = ({color,text,borderColor,textColor,onPress}) => {
  return (
    <div className='customButton' style={
        {
        color: textColor,
        backgroundColor:color,
        borderColor: borderColor,
        }
    } onClick={onPress}>
        {text}
        </div>
  )
}

export default Button