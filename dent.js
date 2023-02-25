import React from "react";
import Webimage1 from "./Rectangle3.png";
import Exp from "./exp.png";

import "./cent.css";

function Dent() {
  return (
    <div>
      <div id="dentcont">
        <img src={Exp} alt="create"></img>
        <h2>Shop for your favorites meal as e dey hot.</h2>
        <p>Shop for your favorite meals or drinks and enjoy while doing it.</p>

        <div id="dentimg">
          <img src={Webimage1} alt="img1"></img>
        </div>
      </div>
    </div>
  );
}
export default Dent;
