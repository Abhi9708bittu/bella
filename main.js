import React from "react";
import Webimage from "./Rectangle.png";
import Webimg from "./rect.png";
import "./main.css";

function Main() {
  return (
    <div>
      <div id="A">
        <img id="a" src={Webimage} alt="img1"></img>
        <img id="b" src={Webimg} alt="img2"></img>
      </div>
    </div>
  );
}
export default Main;
